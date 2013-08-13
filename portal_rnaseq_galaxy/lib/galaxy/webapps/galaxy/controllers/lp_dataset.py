from galaxy.webapps.galaxy.controllers.dataset import *


class HistoryDatasetAssociationListGridEmbed( HistoryDatasetAssociationListGrid ):

    # Grid definition
    #Hack need to pass title in and use _init function
    title = "Choose Files for Your Project"
    model_class = model.HistoryDatasetAssociation
    ##Previously this used the grid template in /dataset/grid.mako NOW I"M USING ?  --Andrew
    template = '/launch_pad/lp_grid_base.mako'
    default_sort_key = "-update_time"
    columns = [
        grids.TextColumn( "Name", key="name", 
                            # Link name to dataset's history.
                           ),
        HistoryDatasetAssociationListGrid.HistoryColumn( "History", key="history", visible=False ),
        #grids.IndividualTagsColumn( "Tags", key="tags", model_tag_association_class=model.HistoryDatasetAssociationTagAssociation, filterable="advanced", grid_name="HistoryDatasetAssocationListGrid" ),
        HistoryDatasetAssociationListGrid.StatusColumn( "Status", key="deleted", attach_popup=False ),
        grids.GridColumn( "Last Updated", key="update_time", format=time_ago ),
    ]
    #Hack to prevent error with History column
    columns[1].sortable=False
    ##columns.append( 
    ##    grids.MulticolFilterColumn(  
    ##    "Search", 
    ##    cols_to_filter=[ columns[0], columns[2] ], 
    ##    key="free-text-search", visible=False, filterable="standard" )
    ##            )
    standard_filters = []
    operations = []
    ## hacking the filter to show Uploaded Files history. Need to set it to only use the Uploaded Files history ID
    default_filter = dict( name="All", deleted="False")
    preserve_state = False ##what does this do? -Andrew
    use_paging = True
    num_rows_per_page = 15
    def build_initial_query( self, trans, **kwargs ):
        # Show user's datasets that are not deleted, not in deleted histories, and not hidden.
        # To filter HDAs by user, need to join model class/HDA and History table so that it is 
        # possible to filter by user. However, for dictionary-based filtering to work, need a 
        # primary table for the query.
        return trans.sa_session.query( self.model_class ).select_from( self.model_class.table.join( model.History.table ) ) \
                .filter( model.History.user == trans.user ) \
                .filter( self.model_class.deleted==False ) \
                .filter( model.History.deleted==False ) \
                .filter( model.History.name=='Uploaded Files' )\
                .filter( self.model_class.visible==True )

    def grid_pass_template(self, trans, grid_template, **kwargs ):
        """grid and associated values can be passed directly to launcpad contents"""
        ##definitely a hack. should pass in kwargs structure at the very least
        ##return a json structure of the data
        grid_args={}
        for k in kwargs.keys():
                grid_args[k]=kwargs[k]
        return (grid_args)

class LPDatasetInterface(DatasetInterface):
    embed_list_grid = HistoryDatasetAssociationListGridEmbed()  

    @web.expose
    @web.require_login( "see all available datasets" )
    def embed_list( self, trans, **kwargs ):
        """List all available datasets"""
        status = message = None
        # Render the list view
        return self.embed_list_grid( trans, status=status, message=message, **kwargs )
        
    @web.expose
    @web.require_login( "see all available datasets" )
    def just_copy_datasets( self, trans, **kwargs ):
        target_history_id=params.get('target_history_id', None)
        if(target_history_id):
            target_histories=trans.webapp.controllers['history'].get_history( trans, target_history_id )
        else:
            target_histories = [ trans.get_history() ]
        if 'operation' in kwargs:
            operation = kwargs['operation'].lower()
            hda_ids = util.listify( kwargs.get( 'id', [] ) )
            
            # Display no message by default
            status, message = None, None

            # Load the hdas and ensure they all belong to the current user
            hdas = []
            target_ids=[]
            for encoded_hda_id in hda_ids:
                hda_id = trans.security.decode_id( encoded_hda_id )
                hda = trans.sa_session.query( model.HistoryDatasetAssociation ).filter_by( id=hda_id ).first()
                if hda:
                    # Ensure history is owned by current user
                    if hda.history.user_id != None and trans.user:
                        assert trans.user.id == hda.history.user_id, "HistoryDatasetAssocation does not belong to current user"
                        if not target_histories[0].has_dataset(hda_id):
                            target_ids.append(hda_id)
                    hdas.append( hda )
                else:
                    log.warn( "Invalid history_dataset_association id '%r' passed to list", hda_id )
        return self._copy_datasets( trans, target_ids, target_histories )
