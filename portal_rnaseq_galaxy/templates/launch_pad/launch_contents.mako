<%inherit file="/tool_form.mako"/>
<%namespace name="lp_grid" file="/launch_pad/lp_grid_base.mako"/>

<%def name="init()">
<%
    self.has_left_panel=False
    self.has_right_panel=False
    self.message_box_visible=False
    self.active_view="user"
    self.overlay_visible=False
%>
</%def>

<%def name="javascripts()">
    ${parent.javascripts()}
    ${self.launch_javascripts()}
</%def>

<%def name="launch_javascripts()">
    <script type="text/javascript">
    ## Hack reset div for search tool that gets mangled in dropping grid directly in
    $(document).ready(function () {
        var search_div =document.getElementById('standard-search');
    });
    function getElement(aID) {
        return (document.getElementById) ?
            document.getElementById(aID) : document.all[aID];
    }

    function getIFrameDocument(aID){ 
        var rv = null; 
        var frame=getElement(aID);
        // if contentDocument exists, W3C 
        // compliant (e.g. Mozilla) 
        if (frame.contentDocument)
            rv = frame.contentDocument;
        else // bad Internet Explorer  ;)
            rv = document.frames[aID].document;
        return rv;
    }

    function adjustMyFrameHeight()
    {
        var frame = getElement("pp_upload");
        var frameDoc = getIFrameDocument("pp_upload");
        frame.height = frameDoc.body.offsetHeight+10;
    }
    $( function() {    
        $( "#launch_configuration2" ).submit(function() {
            $('#launch_configuration1 :input').not(':submit').clone().hide().appendTo('#launch_configuration2');
            return true;
        });
    });
    </script>
</%def>

<%def name="before_body()">
    %if launch_msg:
        <br/>
        <div class="errormessage">${launch_msg}</div>
        <br/>
    %endif

        <div class="grid-header" style="text-align:center">
            <h2>Initialize a Workflow Run</h2>
        </div>

    ##Entry form includes selection forms for datasets, target history, and workflow to run
    <div class="toolForm" id="select_files_chunk">
        <form name="launch_configuration1" id="launch_configuration1">
            
                <div class='form-row'>
                    ${lp_grid.grid_body( grid )}
                </div>
        </form>
     <br/>
</%def>

<%def name="body()">
    ${self.render_launchpad_body( target_histories )}
</%def>

##launchpad body includes grid of datasets, and history, workflow selection
<%def name="render_launchpad_body( target_histories )">
    ##end of "select_files_chunk" div
    </div>
    <%
        universal_share=trans.webapp.controllers['workflow'].get_universal(trans) 
    %>
        <br/>
        <form name="launch_configuration2" id="launch_configuration2" action="${h.url_for( controller='launch_pad', action='launch_workflow')}" method="post">
        <div class="toolForm" id="choose_workflow_chunk" style="float: left; width: 45%; padding: 0px;">
            <div class="grid-header">
                <div class="toolFormTitle"><h2>Choose Workflow</h2></div>
            </div>
            <div class="toolFormBody">
                <div class="form-row" id="workflow list">
                    <select id="single-workflow-select" name="target_workflow_id">
                        <option value=""></option>
                        %for i, association in enumerate(universal_share):
                            <%
                                workflow = association.stored_workflow
                                encoded_id = trans.security.encode_id(workflow.id)
                                source_history_text = ""
                                selected = ""
                                if encoded_id == target_workflow_id:
                                    selected = " selected='selected'"
                            %>
                            <option value="${encoded_id}"${selected}>${i + 1}: ${h.truncate(workflow.name, 30)}</option>
                        %endfor
                    </select><br />
                </div>
            </div>       
        </div>

        <div class="toolForm" id="choose_workflow_chunk" style="float: right; width: 45%; padding: 0px;">
            <div class="grid-header">
                <div class="toolFormTitle"><h2>Choose Destination Project</h2></div>
            </div>

            <div class="toolFormBody">
                <div class="form-row" id="single-destination">
                    <select id="single-dest-select" name="target_history_id">
                        <option value=""></option>
                        %for i, hist in enumerate(target_histories):
                            if hist != current_history:
                                <%
                                    encoded_id = trans.security.encode_id(hist.id)
                                    source_history_text = ""
                                    selected = ""
                                    if hist == source_history:
                                        source_history_text = " (source history)"
                                    if encoded_id == target_history_id:
                                        selected = " selected='selected'"
                                %>
                                <option value="${encoded_id}"${selected}>${i + 1}: ${h.truncate(hist.name, 30)}${source_history_text}</option>
                        %endfor
                    </select><br />
                %if trans.get_user():
                    <hr />
                    <div style="text-align: center; color: #888;">&mdash; OR &mdash;</div>
                    <div class="form-row">
                        <label for="new_history_name" style="display: inline; font-weight:normal;">New Project named:</label>
                        <input type="textbox" name="new_history_name" />
                    </div>
                %endif
                </div>
            </div>
        </div>
        <div style="clear: both"></div>
        <div class="form-row" id="launch_row" align="center"  style="width: 100%">
            <input type="submit" class="primary-button" name="launch_workflow" value="Continue"/>
        </div>
    </form>
</%def>



