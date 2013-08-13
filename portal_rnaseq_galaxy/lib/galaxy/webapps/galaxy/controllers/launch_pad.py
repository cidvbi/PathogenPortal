from galaxy.webapps.galaxy.controllers.history import *
from galaxy.web.base.controller import *
from galaxy.util.bunch import Bunch
from galaxy.tools import DefaultToolState
from galaxy.tools.parameters.basic import UnvalidatedValue
from galaxy.tools.actions import upload_common

import logging
log = logging.getLogger( __name__ )

import pkg_resources
pkg_resources.require( "Routes" )
import routes

class AddFrameData:
    def __init__( self ):
        self.wiki_url = None
        self.debug = None
        self.from_noframe = None
## Need to change this import to be only relevant packages
##For setting up a run of the various workflows and uploading file 

class LaunchPadController( BaseUIController):
    @web.expose
    def index(self, trans, tool_id=None, **kwd):
        """Catches the tool id and redirects as needed"""
        return trans.fill_template( "/launch_pad/index.mako" )
    @web.expose
    def task_menu(self, trans, **kwd):
        return trans.fill_template("/launch_pad/hanger.mako")
    @web.expose
    @web.require_login( "to begin using the RNA-Seq Pipeline" )
    def setup(self, trans, tool_id=None, from_noframe=None, no_help=None, **kwd):
        history=self.set_upload_target( trans )
        current_history= trans.get_history()
        user=trans.get_user()
        ##TODO: restrict access to this section to logged in users
        target_histories=[]
        if user:
            target_histories=trans.sa_session.query( model.History ).filter_by( user=user ).filter(model.History.id != current_history.id)
        grid_args= trans.webapp.controllers['lp_dataset'].embed_list( trans, target_histories=target_histories, show_item_checkboxes='True', **kwd)
        for i in grid_args.keys():
            kwd[i]=grid_args[i]
        kwd['target_histories']=target_histories
        # No tool id passed, redirect to main page
        if tool_id is None:
            tool_id="upload1"
        # Load the tool
        toolbox = self.get_toolbox()
        #Backwards compatibility for datasource tools that have default tool_id configured, but which are now using only GALAXY_URL
        if isinstance( tool_id, list ):
            tool_ids = tool_id
        else:
            tool_ids = [ tool_id ]
        for tool_id in tool_ids:
            tool = toolbox.tools_by_id.get( tool_id, None )
            if tool:
                break
        # No tool matching the tool id, display an error (shouldn't happen)
        if not tool:
            tool_id = ','.join( tool_ids )
            log.error( "index called with tool id '%s' but no such tool exists", tool_id )
            trans.log_event( "Tool id '%s' does not exist" % tool_id )
            return "Tool '%s' does not exist, kwd=%s " % (tool_id, kwd)
        params = util.Params( kwd, sanitize = False ) #Sanitize parameters when substituting into command line via input wrappers
        
        #allow the call tool_runner to disable help
        if no_help:
            tool.help=None
        #do param translation here, used by datasource tools
        if tool.input_translator:
            tool.input_translator.translate( params )
        template, vars = tool.handle_input( trans, params.__dict__ )
        for i in kwd.keys():
            vars[i]=kwd[i]
        if len(params) > 0:
            trans.log_event( "Tool params: %s" % (str(params)), tool_id=tool_id )
        add_frame = AddFrameData()
        add_frame.debug = trans.debug
        if from_noframe is not None:
            add_frame.wiki_url = trans.app.config.wiki_url
            add_frame.from_noframe = True
        return trans.fill_template( "/launch_pad/launch_contents.mako", history=history, toolbox=toolbox, tool=tool, util=util, add_frame=add_frame, **vars )
        



    @web.expose
    @web.require_login( "work with multiple histories" )
    def set_upload_target( self, trans ):
        """Retrieves or creates special Uploaded Files history"""
        user=trans.get_user()
        target_history=None
        for h in trans.sa_session.query( model.History ).filter_by( user=user ):
            #if (hasattr(h, 'upload_target') and h.upload_target):
            if( h.name =='Uploaded Files' and not h.deleted):
                target_history=h
                break
        if (target_history):
            trans.set_history(target_history)
            return target_history
        target_history=trans.new_history(name='Uploaded Files')
        target_history.upload_target = True
        trans.set_history(target_history)
        trans.sa_session.flush()
        return target_history
    #find the workflow based on the tag specified on the hanger page
    #the target_tag needs to correspond to a tag associated with the workflow (in the workflow creation page)
    #the name is selected as the last tag associated with the workflow
    def find_workflow(self, workflow_list, target_tag):
        result={}
        for w in workflow_list:
            found=False
            cur_name=w.stored_workflow.name
            for t in w.stored_workflow.tags:
                if t.user_tname==target_tag:
                    found=True
                    continue
                else:
                    cur_name=t.user_tname
            if found:
                result[cur_name]= w
        return result    
                
                
    @web.expose
    def configure_launch( self, trans, source_history=None, source_dataset_ids="", target_history_id=None, target_history_ids="", new_history_name="", do_copy=False, **kwd ):
        #NOTE: need to get rid of most of the body of this function and call trans.security.encode_id to setup workflow candidates and pass JSON to mako
        #params = util.Params( kwd )
        user = trans.get_user()
        get_api_url=routes.URLGenerator(trans.webapp.api_mapper, trans.environ)
        history_url=get_api_url('histories')#passed to the page for AJAX retrieval via API
        if 'target_workflow_id' in kwd:
            target_workflow_id=kwd['target_workflow_id']
        else:
            target_workflow_id = None
        if 'target_tool_id' in kwd:
            target_tool_id=kwd['target_tool_id']
        else:
            target_tool_id = None        
        if source_history is not None:
            history=trans.webapp.controllers['history'].get_history( trans, source_history )
            #history = self.get_history(trans, source_history)
        else:
            history = trans.get_history()
        refresh_frames = []
        if source_dataset_ids:
            if not isinstance( source_dataset_ids, list ):
                source_dataset_ids = source_dataset_ids.split(",")
            source_dataset_ids = set(map( trans.security.decode_id, source_dataset_ids ))
        else:
            source_dataset_ids = []
        if target_history_id:
            history=trans.webapp.controllers['history'].get_history( trans, target_history_id )
            has_target=True
            #history = self.get_history( trans, target_history_id ) ##this is the call to the UsesHistory class function
            trans.set_history(history)
            target_history_ids = [ trans.security.decode_id(target_history_id) ]
        elif target_history_ids:
            if not isinstance( target_history_ids, list ):
                target_history_ids = target_history_ids.split(",")
            target_history_ids = set([ trans.security.decode_id(h) for h in target_history_ids if h ])
        else:
            target_history_ids = []
        done_msg = error_msg = ""
        new_history = None
  
        source_datasets = history.visible_datasets
        target_histories = [history]
        if user:
           target_histories = user.active_histories
        if kwd['retrieval_tag']=="ebi_sra_main":
            toolbox=self.get_toolbox()
            tool = toolbox.tools_by_id.get("ebi_sra_main", None )
            link = url_for( tool.action, **tool.get_static_param_values( trans ) )
            candidate_tools=kwd['candidate_tools']
            #hack push url and tool id through candidate tools
            candidate_tools[link]="ebi_sra_main"

        if target_workflow_id:
            ##launch workflow
            return trans.response.send_redirect( web.url_for( controller='root', workflow_id=target_workflow_id, refresh_frames=['history']) )
        elif target_tool_id:
            return trans.response.send_redirect( web.url_for( controller='root', tool_id=target_tool_id, refresh_frames=['history']) )
        else:
            return trans.fill_template( "/launch_pad/launch_configure.mako",
                                    source_history = history,
                                    current_history = trans.get_history(),
                                    source_dataset_ids = source_dataset_ids,
                                    target_history_id = target_history_id,
                                    target_history_ids = target_history_ids,
                                    source_datasets = source_datasets,
                                    target_histories = target_histories,
                                    new_history_name = new_history_name,
                                    done_msg = done_msg,
                                    error_msg = error_msg,
                                    refresh_frames = refresh_frames, retrieval_tag=kwd['retrieval_tag'], candidate_workflows=kwd['candidate_workflows'], candidate_tools=kwd['candidate_tools'],
                                    history_url=history_url)
        
    @web.expose
    @web.require_login( "to configure a workflow" )
    def leave_hanger(self, trans, **kwargs):
        user=trans.get_user()
        ##TODO: restrict access to this section to logged in users
        #target_histories=[]
        candidate_workflows={}
        candidate_tools={}
        #if user:
        #    target_histories=trans.sa_session.query( model.History ).filter_by( user=user #)#.filter(model.History.id != current_history.id)
        params = util.Params(kwargs)
        launch_type=params.get('launch_type',None) #should be tool or workflow
        retrieval_tag=params.get('retrieval_tag',None) #workflows need a retrieval_tag
        tool_list=params.get('tool_list',None) #see if there is a list of tools
        if(launch_type=="upload"):
            history=self.set_upload_target( trans )
            if (retrieval_tag=="upload1"):
                return trans.response.send_redirect( web.url_for( controller='tool_runner', action='index', tool_id=retrieval_tag, from_noframe=None, no_help=None ) )
            
        #if the launch type is a workflow get the eligible ones according to their tags
        universal_share=trans.webapp.controllers['workflow'].get_universal(trans)
        if(launch_type=="workflow" and retrieval_tag!=None):
            candidate_workflows=self.find_workflow(universal_share,retrieval_tag)
        if(launch_type=="tool_list" and tool_list != None):
            parts=tool_list.split(',')
            for p in parts:
                subparts=p.split(':')
                if len(subparts)==2:
                    tmp_workflows=self.find_workflow(universal_share,subparts[1])
                    candidate_workflows.update(tmp_workflows)
                    if len(tmp_workflows) == 0:
                        candidate_tools[subparts[0]]=subparts[1]
        return self.configure_launch(trans, candidate_workflows=candidate_workflows, candidate_tools=candidate_tools, **kwargs)
                
        
    @web.expose
    @web.require_login( "see all available datasets" )
    def launch_workflow( self, trans, **kwargs ):
        params = util.Params( kwargs )
        target_history_id=params.get('target_history_id', None)
        new_history_name=params.get('new_history_name', "")
        data_ids=params.get('id', None)
        target_workflow_id=params.get('target_workflow_id', None)
        has_target=False
        has_data=False
        if target_history_id and len(new_history_name)==0 and target_history_id!='?':
            ##select this as current history
            history=trans.webapp.controllers['history'].get_history( trans, target_history_id )
            has_target=True
            #history = self.get_history( trans, target_history_id ) ##this is the call to the UsesHistory class function
            trans.set_history(history)
        elif new_history_name and len(new_history_name)>0:
            ##create the new history
            has_target=True
            history = trans.new_history(new_history_name)
        if data_ids and has_target:
            ##copy these datasets to selected history
            has_data=True
            kwargs['operation']="copy to current history"
            trans.webapp.controllers['lp_dataset'].just_copy_datasets( trans, **kwargs)
        if target_workflow_id and has_target and has_data:
            ##launch workflow
            return trans.response.send_redirect( web.url_for( controller='workflow', action='run', id=target_workflow_id) )
        else:
            return trans.response.send_redirect( web.url_for( controller='launch_pad', action='setup', tool_id='upload1', launch_msg='You must specify 1. Input Files, 2. A Workflow, and 3. A Project Name') )
    @web.expose
    def rerun( self, trans, id=None, from_noframe=None, **kwd ):
        """
        Given a HistoryDatasetAssociation id, find the job and that created
        the dataset, extract the parameters, and display the appropriate tool
        form with parameters already filled in.
        """
        if not id:
            error( "'id' parameter is required" );
        try:
            id = int( id )
        except:
            error( "Invalid value for 'id' parameter" )
        # Get the dataset object
        data = trans.sa_session.query( trans.app.model.HistoryDatasetAssociation ).get( id )
        #only allow rerunning if user is allowed access to the dataset.
        if not trans.app.security_agent.can_access_dataset( trans.get_current_user_roles(), data.dataset ):
            error( "You are not allowed to access this dataset" )
        # Get the associated job, if any. If this hda was copied from another,
        # we need to find the job that created the origial hda
        job_hda = data
        while job_hda.copied_from_history_dataset_association:#should this check library datasets as well?
            job_hda = job_hda.copied_from_history_dataset_association
        if not job_hda.creating_job_associations:
            error( "Could not find the job for this dataset" )
        # Get the job object
        job = None
        for assoc in job_hda.creating_job_associations:
            job = assoc.job
            break   
        if not job:
            raise Exception("Failed to get job information for dataset hid %d" % data.hid)
        # Get the tool object
        tool_id = job.tool_id
        try:
            # Load the tool
            toolbox = self.get_toolbox()
            tool = toolbox.tools_by_id.get( tool_id, None )
            assert tool is not None, 'Requested tool has not been loaded.'
        except:
            #this is expected, so not an exception
            error( "This dataset was created by an obsolete tool (%s). Can't re-run." % tool_id )
        # Can't rerun upload, external data sources, et cetera. Workflow
        # compatible will proxy this for now
        if not tool.is_workflow_compatible:
            error( "The '%s' tool does not currently support rerunning." % tool.name )
        # Get the job's parameters
        try:
            params_objects = job.get_param_values( trans.app, ignore_errors = True )
        except:
            raise Exception( "Failed to get parameters for dataset id %d " % data.id )
        upgrade_messages = tool.check_and_update_param_values( params_objects, trans )
        # Need to remap dataset parameters. Job parameters point to original 
        # dataset used; parameter should be the analygous dataset in the 
        # current history.
        history = trans.get_history()
        hda_source_dict = {} # Mapping from HDA in history to source HDAs.
        for hda in history.datasets:
            source_hda = hda.copied_from_history_dataset_association
            while source_hda:#should this check library datasets as well?
                #FIXME: could be multiple copies of a hda in a single history, this does a better job of matching on cloned histories, 
                #but is still less than perfect when eg individual datasets are copied between histories
                if source_hda not in hda_source_dict or source_hda.hid == hda.hid:
                    hda_source_dict[ source_hda ] = hda
                source_hda = source_hda.copied_from_history_dataset_association
        # Unpack unvalidated values to strings, they'll be validated when the
        # form is submitted (this happens when re-running a job that was
        # initially run by a workflow)
        #This needs to be done recursively through grouping parameters
        def rerun_callback( input, value, prefixed_name, prefixed_label ):
            if isinstance( value, UnvalidatedValue ):
                return str( value )
            if isinstance( input, DataToolParameter ):
                if value not in history.datasets and value in hda_source_dict:
                    return hda_source_dict[ value ]
        visit_input_values( tool.inputs, params_objects, rerun_callback )
        # Create a fake tool_state for the tool, with the parameters values 
        state = tool.new_state( trans )
        state.inputs = params_objects
        tool_state_string = util.object_to_string(state.encode(tool, trans.app))
        # Setup context for template
        vars = dict( tool_state=state, errors = upgrade_messages )
        # Is the "add frame" stuff neccesary here?
        add_frame = AddFrameData()
        add_frame.debug = trans.debug
        if from_noframe is not None:
            add_frame.wiki_url = trans.app.config.wiki_url
            add_frame.from_noframe = True
        return trans.fill_template( "tool_form.mako", history=history, toolbox=toolbox, tool=tool, util=util, add_frame=add_frame, **vars )
    @web.expose
    def redirect( self, trans, redirect_url=None, **kwd ):
        if not redirect_url:
            return trans.show_error_message( "Required URL for redirection missing" )
        trans.log_event( "Redirecting to: %s" % redirect_url )
        return trans.fill_template( 'root/redirect.mako', redirect_url=redirect_url )
    @web.json
    def upload_async_create( self, trans, tool_id=None, **kwd ):
        """
        Precreate datasets for asynchronous uploading.
        """
        cntrller = kwd.get( 'cntrller', '' )
        roles = kwd.get( 'roles', False )
        if roles:
            # The user associated the DATASET_ACCESS permission on the uploaded datasets with 1 or more roles.
            # We need to ensure that the roles are legitimately derived from the roles associated with the LIBRARY_ACCESS
            # permission if the library is not public ( this should always be the case since any ill-legitimate roles
            # were filtered out of the roles displayed on the upload form.  In addition, we need to ensure that the user
            # did not associated roles that would make the dataset in-accessible by everyone.
            library_id = trans.app.security.decode_id( kwd.get( 'library_id', '' ) )
            vars = dict( DATASET_ACCESS_in=roles )
            permissions, in_roles, error, msg = trans.app.security_agent.derive_roles_from_access( trans, library_id, cntrller, library=True, **vars )
            if error:
                return [ 'error', msg ]
        def create_dataset( name ):
            ud = Bunch( name=name, file_type=None, dbkey=None )
            if nonfile_params.get( 'folder_id', False ):
                replace_id = nonfile_params.get( 'replace_id', None )
                if replace_id not in [ None, 'None' ]:
                    replace_dataset = trans.sa_session.query( trans.app.model.LibraryDataset ).get( trans.security.decode_id( replace_id ) )
                else:
                    replace_dataset = None
                # FIXME: instead of passing params here ( chiech have been process by util.Params(), the original kwd
                # should be passed so that complex objects that may have been included in the initial request remain.
                library_bunch = upload_common.handle_library_params( trans, nonfile_params, nonfile_params.folder_id, replace_dataset )
            else:
                library_bunch = None
            return upload_common.new_upload( trans, cntrller, ud, library_bunch=library_bunch, state=trans.app.model.HistoryDatasetAssociation.states.UPLOAD )
        tool = self.get_toolbox().tools_by_id.get( tool_id, None )
        if not tool:
            return False # bad tool_id
        nonfile_params = util.Params( kwd, sanitize=False )
        if kwd.get( 'tool_state', None ) not in ( None, 'None' ):
            encoded_state = util.string_to_object( kwd["tool_state"] )
            tool_state = DefaultToolState()
            tool_state.decode( encoded_state, tool, trans.app )
        else:
            tool_state = tool.new_state( trans )
        errors = tool.update_state( trans, tool.inputs, tool_state.inputs, kwd, update_only = True )
        datasets = []
        dataset_upload_inputs = []
        for input_name, input in tool.inputs.iteritems():
            if input.type == "upload_dataset":
                dataset_upload_inputs.append( input )
        assert dataset_upload_inputs, Exception( "No dataset upload groups were found." )
        for dataset_upload_input in dataset_upload_inputs:
            d_type = dataset_upload_input.get_datatype( trans, kwd )
            
            if d_type.composite_type is not None:
                datasets.append( create_dataset( dataset_upload_input.get_composite_dataset_name( kwd ) ) )
            else:
                params = Bunch( ** tool_state.inputs[dataset_upload_input.name][0] )
                if params.file_data not in [ None, "" ]:
                    name = params.file_data
                    if name.count('/'):
                        name = name.rsplit('/',1)[1]
                    if name.count('\\'):
                        name = name.rsplit('\\',1)[1]
                    datasets.append( create_dataset( name ) )
                if params.url_paste not in [ None, "" ]:
                    url_paste = params.url_paste.replace( '\r', '' ).split( '\n' )
                    url = False
                    for line in url_paste:
                        line = line.rstrip( '\r\n' ).strip()
                        if not line:
                            continue
                        elif line.lower().startswith( 'http://' ) or line.lower().startswith( 'ftp://' ) or line.lower().startswith( 'https://' ):
                            url = True
                            datasets.append( create_dataset( line ) )
                        else:
                            if url:
                                continue # non-url when we've already processed some urls
                            else:
                                # pasted data
                                datasets.append( create_dataset( 'Pasted Entry' ) )
                                break
        return [ d.id for d in datasets ]

    @web.expose
    def upload_async_message( self, trans, **kwd ):
        # might be more appropriate in a different controller
        msg = """<p>Your upload has been queued.  History entries that are still uploading will be blue, and turn green upon completion.</p>
        <p><b>Please do not use your browser\'s "stop" or "reload" buttons until the upload is complete, or it may be interrupted.</b></p>
        <p>You may safely continue to use Galaxy while the upload is in progress.  Using "stop" and "reload" on pages other than Galaxy is also safe.</p>
        """
        return trans.show_message( msg, refresh_frames='history', use_panels=True )
