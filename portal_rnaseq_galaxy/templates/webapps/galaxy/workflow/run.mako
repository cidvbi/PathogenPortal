<%inherit file="/base.mako"/>

<%def name="javascripts()">
    ${parent.javascripts()}
    ${h.js( "libs/jquery/jquery.autocomplete", "libs/bootstrap" )}
    ##<script src="/static/scripts/extjs-4/ext-all.js" type="text/javascript"></script>
    ##"LiveSearchGridPanel", "StatusBar"
    <%
        from mako.runtime import TemplateNamespace
        ## launch_pad display types supported
        ## preprocess_display()
    %>

    <script type="text/javascript">
        function selectAllOptions()
        {
            var all_equal=true;
            var prev_len=-1;
            var cur_len =1;
            $('select[name*="|input"]').each(function(i, s) {
                  var select = $(s);
                  var interaction_type = select.attr('interaction_type');
                  multi = select.attr('multiple');
                  if(typeof interaction_type === 'undefined' && typeof multi !== 'undefined' && multi !== false){
                      interaction_type='normal_multi';
                  }
                  if(interaction_type == 'normal_multi' || interaction_type == 'on_deck2'){
                      if (interaction_type == 'on_deck2'){
                          select.find('option').attr('selected', 'selected');
                      }
                      if(prev_len ==-1){
                          prev_len= cur_len=select.find('option:selected').length;
                      }
                      else{
                          cur_len=select.find('option:selected').length;
                      }
                      all_equal=prev_len == cur_len && all_equal;
                      prev_len=cur_len;
                  }
            });
            if( all_equal ){
                return(cur_len);
            }
            else{
                return(-1);
            }
        }
        function addOption(theSel, theText, theValue)
        {
            var newOpt = new Option(theText, theValue);
            var selLength = theSel.length;
            theSel.options[selLength] = newOpt;
        }

        function deleteOption(theSel, theIndex)
        { 
            var selLength = theSel.length;
            if(selLength>0)
            {
              theSel.options[theIndex] = null;
            }
        }
        function copyOptions(from_elm, to_elm)
        {
            var theSelFrom = document.getElementsByName(from_elm)[0]
            var theSelTo = document.getElementsByName(to_elm)[0]
            var selLength = theSelFrom.length;
            var selectedText = new Array();
            var selectedValues = new Array();
            var selectedCount = 0;
        
            var i;
            
            // Find the selected Options in reverse order
            // and delete them from the 'from' Select.
            for(i=selLength-1; i>=0; i--)
            {
                if(theSelFrom.options[i].selected)
                {
                    selectedText[selectedCount] = theSelFrom.options[i].text;
                    selectedValues[selectedCount] = theSelFrom.options[i].value;
                    selectedCount++;
                }
            }
            
            // Add the selected text/values in reverse order.
            // This will add the Options to the 'to' Select
            // in the same order as they were in the 'from' Select.
            for(i=selectedCount-1; i>=0; i--)
            {
                addOption(theSelTo, selectedText[i], selectedValues[i]);
            }
        
        }
        function removeOptions(from_elm)
        {
            var theSelFrom = document.getElementsByName(from_elm)[0]
            var selLength = theSelFrom.length;
            var selectedText = new Array();
            var selectedValues = new Array();
            var selectedCount = 0;
        
            var i;
            
            // Find the selected Options in reverse order
            // and delete them from the 'from' Select.
            for(i=selLength-1; i>=0; i--)
            {
                if(theSelFrom.options[i].selected)
                {
                    deleteOption(theSelFrom, i);
                    selectedCount++;
                }
            }
        }
        function moveOptions(from_elm, to_elm)
        {
            var theSelFrom = document.getElementsByName(from_elm)[0]
            var theSelTo = document.getElementsByName(to_elm)[0]
            var selLength = theSelFrom.length;
            var selectedText = new Array();
            var selectedValues = new Array();
            var selectedCount = 0;
        
            var i;
            
            // Find the selected Options in reverse order
            // and delete them from the 'from' Select.
            for(i=selLength-1; i>=0; i--)
            {
                if(theSelFrom.options[i].selected)
                {
                    selectedText[selectedCount] = theSelFrom.options[i].text;
                    selectedValues[selectedCount] = theSelFrom.options[i].value;
                    deleteOption(theSelFrom, i);
                    selectedCount++;
                }
            }
            
            // Add the selected text/values in reverse order.
            // This will add the Options to the 'to' Select
            // in the same order as they were in the 'from' Select.
            for(i=selectedCount-1; i>=0; i--)
            {
                addOption(theSelTo, selectedText[i], selectedValues[i]);
            }
        
        }
        $( function() {
            function show_tool_body(title){
                title.parent().show().css('border-bottom-width', '1px');
                title.next().show('fast');
                if ('${hide_fixed_params}'.toLowerCase() == 'true') {
                    // show previously hidden parameters
                    title.next().children(".form-row").show();
                }
            }
            function hide_tool_body(title){
                title.parent().css('border-bottom-width', '0px');
                title.next().hide('fast');
            }
            function toggle_tool_body(title) {
                if (title.next().is(':visible')){
                    hide_tool_body(title);
                }else{
                    show_tool_body(title);
                }
            }
            function toggle_multiinput(select) {
                var placeholder;
                if (select.attr('multiple')) {
                    //$('.multiinput').removeClass('disabled');
                    if (select.val()) {
                        select.val(select.val()[0]);
                    } else {
                        select.val($('option:last', select).val());
                    }
                    select.removeAttr('multiple').removeAttr('size');
                    placeholder = 'type to filter';
                } else {
                    //$('.multiinput').addClass('disabled');
                    //$('.multiinput', select.closest('.form-row')).removeClass('disabled');
                    select.attr('multiple', 'multiple').attr('size', 8);
                    placeholder = 'type to filter, [enter] to select all';
                }
                $('input.multiinput-filter', select.parent()).attr(
                    'placeholder', placeholder);
            }
            $( "select[refresh_on_change='true']").change( function() {
                $( "#tool_form" ).submit();
            });
            $( "#tool_form" ).submit(function() {
                var in_length=selectAllOptions();
                if(in_length == -1){
                    return(false);
                }
                else if(in_length == 0){
                    return(false);
                }
            });
            $("div.toolFormTitle").click(function(){
                toggle_tool_body($(this));
            });
            $(".details").click(function(e) {
                e.preventDefault();
                var detailsDiv = document.getElementById(this.attributes.div_target.value);
                $(detailsDiv).toggle();
            });
            if ('${hide_fixed_params}'.toLowerCase() == 'true') {
                // hide parameters that are not runtime inputs
                $("div.form-row:not(:has(select, textarea, input[type!=hidden], .wfpspan))").hide();
                $("div.toolForm:not(:has(select, textarea, input[type!=hidden], .wfpspan))").hide();
            }
            else {
                // Collapse non-interactive run-workflow panels by default.
                $("div.toolFormBody:not(:has(select, textarea, input[type!=hidden], .wfpspan))").hide().parent().css('border-bottom-width', '0px');
            }
            $("#show_all_tool_body").click(function(){
                $("div.toolFormTitle").each(function(){
                    show_tool_body($(this));
                });
            });
            $("#hide_all_tool_body").click(function(){
                $("div.toolFormTitle").each(function(){
                    hide_tool_body($(this));
                });
            });
            $("#new_history_cbx").click(function(){
                $("#new_history_input").toggle(this.checked);
            });
            $('span.multiinput_wrap select[name*="|input"]').removeAttr('multiple').each(function(i, s) {
                var select = $(s);
                var new_width = Math.max(200, select.width()) + 20;
                var is_on_deck = false;
                var interaction_type = select.attr('interaction_type');
                if(typeof interaction_type !== 'undefined' && interaction_type.lastIndexOf('on_deck', 0) === 0){
                    is_on_deck =true;
                }
                if( ! is_on_deck){
                    select.closest('.form-row').children('label').append(
                        $('<span class="icon-button multiinput"></span>').click(function() {
                            if ($(this).hasClass('disabled')) return;
                            toggle_multiinput(select);
                            select.focus();
                        }).attr('original-title',
                                'Enable/disable selection of multiple input ' +
                                'files. Each selected file will have an ' +
                                'instance of the workflow.').tooltip({placement: 'bottom'})
                    );
                }
                if(! is_on_deck || (is_on_deck && interaction_type=='on_deck1')){
                    var filter = $('<input type="text" class="multiinput-filter" ' +
                                  'placeholder="type to filter">');
                    var filter_timeout = false;
                    var original_rows = select.find('option');
                    var previous_filter = '';
                    // Todo: might have to choose keypress, depending on browser
                    filter.keydown(function(e) {
                        var filter_select = function() {
                            var f = $.trim(filter.val());
                            var filtered_rows = original_rows;
                            if (f.length >= 1) {
                                filtered_rows = original_rows.filter(function() {
                                    return new RegExp(f, 'ig').test($(this).text());
                                });
                            }
                            select.html('');
                            select.html(filtered_rows);
                        };
                        if (e.which == 13) { // 13 = enter key
                            e.preventDefault();
                            multi = select.attr('multiple');
                            if (typeof multi !== 'undefined' && multi !== false) {
                                if (!select.find('option:not(:selected)').length) {
                                    select.find('option').removeAttr('selected');
                                } else {
                                    select.find('option').attr('selected', 'selected');
                                }
                            }
                            return;
                        }
                        if (filter.val() != previous_filter) {
                            if (filter_timeout) clearTimeout(filter_timeout);
                            timeout = setTimeout(filter_select, 300);
                            previous_filter = filter.val();
                        }
                    }).width(new_width).css('display', 'block');
                    select.after(filter);
                }
                select.width(new_width);
                if(is_on_deck){
                    toggle_multiinput(select);
                }
            });
        });
    </script>
</%def>

<%def name="stylesheets()">
    ${parent.stylesheets()}
    ${h.css( "autocomplete_tagging" )}
    ##ExtJS css
    ##TODO should be moved into a conditional that ExtJS will actually be used.
    <link href="/static/scripts/extjs-4/resources/css/ext-all-gray.css" media="screen" rel="Stylesheet" type="text/css">
    <link href="/static/scripts/extjs-4/examples/ux/css/LiveSearchGridPanel.css" media="screen" rel="Stylesheet" type="text/css">
    <link href="/static/scripts/extjs-4/examples/ux/statusbar/css/statusbar.css" media="screen" rel="Stylesheet" type="text/css">
    <style type="text/css">
    #new_history_p{
        line-height:2.5em;
        margin:0em 0em .5em 0em;
    }
    #new_history_cbx{
        margin-right:.5em;
    }
    #new_history_input{
        display:none;
        line-height:1em;
    }
    #ec_button_container{
        float:right;
    }
    div.toolForm{
        margin-top: 10px;
        margin-bottom: 10px;
    }
    div.toolFormTitle{
        cursor:pointer;
    }
    .title_ul_text{
        text-decoration:underline;
    }
    .step-annotation {
        margin-top: 0.25em;
        font-weight: normal;
        font-size: 97%;
    }
    .workflow-annotation {
        margin-bottom: 1em;
    }
    /* This should probably be moved into a class and associated with specific divs */
    .x-grid-row-over .x-grid-cell-inner {
        font-weight: bold;
    }
    .x-panel-body {
        border-color: #EBD9B2;
    }
 

    </style>
</%def>

<%!
from galaxy.tools.parameters import DataToolParameter, RuntimeValue, ShellSelectToolParameter
from galaxy.jobs.actions.post import ActionBox
uniq_display_types=set()
uniq_display_namespaces=set()
%>

<%def name="register_display( display_type, step_id, data_store, namespace_index )">
    <%
        if display_type in namespace_index:
            display_elements[step_id]={'display_type':display_type, 'data_store':data_store, 'namespace':namespace_index[display_type]}
    %>
</%def>

<%def name="crawl_inputs(inputs, values, prefix, step, namespace_index)">
    <%
        for input_index, input in enumerate( inputs.itervalues() ):
            if input.type == "conditional":
                group_values = values[input.name]
                current_case = group_values['__current_case__']
                new_prefix = prefix + input.name + "|"
                crawl_inputs( input.cases[ current_case ].inputs, group_values, new_prefix, step, namespace_index)
            elif isinstance( input, ShellSelectToolParameter ):
                ##if this is a Shell Parameter then expand the javascript based on the display type and create unique variables based on the step
                data_string = input.get_data_string()
                shell_display_id=str(step.id)+"|"+prefix+input.name
                register_display(input.display, shell_display_id, data_string, namespace_index)
    %>
</%def>

<%def name="preprocess_display()">
    <%
        ##HACK putting namespace_index here because it needs to be declared after lp_search_select namespace is created but before its used
        ## it can't go in the module level block since its called before the namespace is created. This wouldn't be a problem if you used strings
        ## to lookup the namespace instead of storing the namespace itself...
        namespace_index={'search_select':lp_search_select}
        for step in steps:
            if step.type == 'tool' or step.type is None:
                tool = trans.app.toolbox.get_tool( step.tool_id )
                for input_index, input in enumerate( tool.inputs.itervalues() ):
                    if input.type =="conditional":
                        group_values = step.state.inputs[input.name]
                        current_case = group_values['__current_case__']
                        new_prefix = input.name + "|"
                        crawl_inputs( input.cases[ current_case ].inputs, group_values, new_prefix, step, namespace_index)
                    elif input.type != "repeat":
                        value = step.state.inputs[ input.name]
                        prefix=""
                        if isinstance( value, RuntimeValue ) or ( str(step.id) + '|__runtime__' + prefix + input.name ) in incoming:
                            ##Check for shell instance of parameters HACK: this should be a more generic base type for Shell Parameters
                            if isinstance( input, ShellSelectToolParameter ):
                                ##if this is a Shell Parameter then expand the javascript based on the display type and create unique variables based on the step
                                data_string = input.get_data_string()
                                shell_display_id=str(step.id)+"|"+prefix+input.name
                                register_display(input.display, shell_display_id, data_string, namespace_index)
    %>
</%def>

<%
import re
import colorsys
import random

used_accumulator = []

wf_parms = {}
for step in steps:
    for v in [ActionBox.get_short_str(pja) for pja in step.post_job_actions] + step.state.inputs.values():
        if isinstance(v, basestring):
            for rematch in re.findall('\$\{.+?\}', v):
                if rematch[2:-1] not in wf_parms:
                    wf_parms[rematch[2:-1]] = ""
if wf_parms:
    hue_offset = 1.0 / len(wf_parms)
    hue = 0.0
    for k in wf_parms.iterkeys():
        wf_parms[k] = "#%X%X%X" % tuple([int(x * 255) for x in colorsys.hsv_to_rgb(hue, .1, .9)])
        hue += hue_offset
%>


<%def name="expand_js()">
    ##get the types of displays for this workflow
    <% 
        uniq_display_types = set([elem['display_type'] for elem in display_elements.values()])
    %>
    ##Extjs section
    %if len(uniq_display_types):
        <script type="text/javascript">
        ##(function() {
        ##Ext.BLANK_IMAGE_URL = '/static/scripts/extjs-4/resources/themes/images/default/s.gif';
        Ext.Loader.setConfig({enabled: true});
        ##HACK should use programmati
        Ext.Loader.setPath('Ext', '/static/scripts/extjs-4/');
        Ext.Loader.setPath('Ext.ux', '/static/scripts/extjs-4/examples/ux/');
        ##Ext.Loader.setPath('Ext.ux.statusbar', '/static/scripts/');
        <%
            uniq_display_namespaces=set([elem['namespace'] for elem in display_elements.values()])
            ##expand general js
            expand_general_functions( 'general_functions', uniq_display_namespaces )
            ##expand extjs require
            extjs_requirements(uniq_display_namespaces)
            ##expand specific js
            for id, elem in display_elements.items(): 
                elem['namespace'].specific_functions(id)
            ##expand extjs on ready section
            extjs_on_ready(uniq_display_namespaces)
        %>
        ##})();
        </script>
    %endif 
</%def>

<%def name="extjs_requirements(uniq_display_namespaces)">
    ##get all the extjs_requirements from all the different display types and create the extjs require sectoin
    ##expects unique list of display types
    <%
        extjs_includes=set()
    %>
    ##get the strings for the extjs require block
    %for n in uniq_display_namespaces:
        <%
            for req in n.extjs_requires():
                extjs_includes.add(req)
        %>
    %endfor
    Ext.require(${list(extjs_includes)});
</%def> 

<%def name="expand_general_functions( retrieval_func , uniq_display_namespaces)">
    ## Get a list of mako function names based on the retrieval_func, then expand each unique one into javascript
    <%
        general_funcs_id=set()
    %>
    %for n in uniq_display_namespaces:
        <% get_names=getattr(n, retrieval_func) %>
            %for req in get_names():
                %if not req in general_funcs_id:
                     ${n.template.get_def(req).render()}
                %endif
                <%general_funcs_id.add(req)%>
            %endfor

    %endfor
</%def> 

<%def name="extjs_on_ready(uniq_display_namespaces )">
    Ext.onReady(function() {
    ${expand_general_functions( 'general_onready_functions' , uniq_display_namespaces)}
    %for id, elem in display_elements.items():
        ${elem['namespace'].specific_onready_functions(id, elem['data_store'])}
    %endfor
    });
</%def> 

<%def name="do_inputs( inputs, values, errors, prefix, step, other_values = None, already_used = None )">
  %if other_values is None:
      <% other_values = values %>
  %endif
  %for input_index, input in enumerate( inputs.itervalues() ):
    %if input.type == "repeat":
      <div class="repeat-group">
          <div class="form-title-row"><b>${input.title_plural}</b></div>
              %if input.help:
                  <div class="toolParamHelp" style="clear: both;">
                      ${input.help}
                  </div>
              %endif
          <% repeat_values = values[input.name] %>
          %for i in range( len( repeat_values ) ):
            %if input.name in errors:
                <% rep_errors = errors[input.name][i] %>
            %else:
                <% rep_errors = dict() %>
            %endif
            <div class="repeat-group-item">
            <% index = repeat_values[i]['__index__'] %>
            <div class="form-title-row"><b>${input.title} ${i + 1}</b></div>
            ${do_inputs( input.inputs, repeat_values[ i ], rep_errors,  prefix + input.name + "_" + str(index) + "|", step, other_values, already_used )}
            ## <div class="form-row"><input type="submit" name="${step.id}|${prefix}${input.name}_${i}_remove" value="Remove ${input.title} ${i+1}" /></div>
            </div>
          %endfor
          ## <div class="form-row"><input type="submit" name="${step.id}|${prefix}${input.name}_add" value="Add new ${input.title}" /></div>
      </div>
    %elif input.type == "conditional":
      <% group_values = values[input.name] %>
      <% current_case = group_values['__current_case__'] %>
      <% new_prefix = prefix + input.name + "|" %>
      <% group_errors = errors.get( input.name, {} ) %>
      ${row_for_param( input.test_param, group_values[ input.test_param.name ], other_values, group_errors, prefix, step, already_used )}
      ${do_inputs( input.cases[ current_case ].inputs, group_values, group_errors, new_prefix, step, other_values, already_used )}
    %else:
      ${row_for_param( input, values[ input.name ], other_values, errors, prefix, step, already_used )}
    %endif
  %endfor
</%def>

<%def name="row_for_param( param, value, other_values, error_dict, prefix, step, already_used )">
    ## -- ${param.name} -- ${step.state.inputs} --
    <% help_id=str(step.id)+"|"+param.name+"|info" %>
    %if error_dict.has_key( param.name ):
        <% cls = "form-row form-row-error" %>
    %else:
        <% cls = "form-row" %>
    %endif
    <div class="${cls}">
        %if param.help and len(param.help) > 120:
            <table><tr height="20"><td><label>${param.get_label()}</label></td><td width="15"></td><td valign="top"><a href="" div_target="${help_id}" class="icon-button information details"></a></td></tr></table>
        %else:
            <label>${param.get_label()}</label>
        %endif
        <div>
            %if isinstance( param, DataToolParameter ):
                %if ( prefix + param.name ) in step.input_connections_by_name:
                    <%
                        conns = step.input_connections_by_name[ prefix + param.name ]
                        if not isinstance(conns, list):
                            conns = [conns]
                        vals = ["Output dataset '%s' from step %d" % (conn.output_name, int(conn.output_step.order_index)+1) for conn in conns]
                    %>
                    ${",".join(vals)}
                %else:
                    ## FIXME: Initialize in the controller
                    <%
                    if value is None:
                        value = other_values[ param.name ] = param.get_initial_value_from_history_prevent_repeats( t, other_values, already_used )
                        if not enable_unique_defaults:
                            del already_used[:]
                    %>
                    %if step.type == 'data_input':
                    ##Input Dataset Step, wrap for multiinput.
                        <span class='multiinput_wrap'>
                        ${param.get_html_field( t, value, other_values ).get_html( str(step.id) + "|" + prefix )}
                        </span>
                    %else:
                        ${param.get_html_field( t, value, other_values ).get_html( str(step.id) + "|" + prefix )}
                    %endif

                    <input type="hidden" name="${step.id}|__force_update__${prefix}${param.name}" value="true" />
                %endif
            %elif isinstance( value, RuntimeValue ) or ( str(step.id) + '|__runtime__' + prefix + param.name ) in incoming:
                ## On the first load we may see a RuntimeValue, so we write
                ## an input field using the initial value for the param.
                ## Subsequents posts will no longer have the runtime value
                ## (since an actualy value will be posted) so we add a hidden
                ## field so we know to continue drawing form for this param.
                ## FIXME: This logic shouldn't be in the template. The
                ## controller should go through the inputs on the first
                ## load, fill in initial values where needed, and mark
                ## all that are runtime modifiable in some way.
                <%
                    value = other_values[ param.name ] = param.get_initial_value_from_history_prevent_repeats( t, other_values, already_used )
                    if not enable_unique_defaults:
                        del already_used[:]         
                %>
                ${param.get_html_field( t, value, other_values ).get_html( str(step.id) + "|" + prefix )}
                <input type="hidden" name="${step.id}|__runtime__${prefix}${param.name}" value="true" />
            %else:              
                <%
                p_text = param.value_to_display_text( value, app )
                replacements = []
                if isinstance(p_text, basestring):
                    for rematch in re.findall('\$\{.+?\}', p_text):
                        replacements.append('wf_parm__%s' % rematch[2:-1])
                        p_text = p_text.replace(rematch, '<span style="background-color:%s" class="wfpspan wf_parm__%s">%s</span>' % (wf_parms[rematch[2:-1]], rematch[2:-1], rematch[2:-1]))
                %>
                %if replacements:
                    <span style="display:none" class="parm_wrap ${' '.join(replacements)}">
                    ${param.get_html_field( t, value, other_values ).get_html( str(step.id) + "|" + prefix )}
                    </span>
                    <span class="p_text_wrapper">${p_text}</span>
                    <input type="hidden" name="${step.id}|__runtime__${prefix}${param.name}" value="true" />
                %else:
                    ${param.value_to_display_text( value, app )}
                %endif
            %endif
        </div>
        %if step.upgrade_messages and param.name in step.upgrade_messages:
        <div class="warningmark">${step.upgrade_messages[param.name]}</div>
        %endif
        %if error_dict.has_key( param.name ):
        <div style="color: red; font-weight: bold; padding-top: 1px; padding-bottom: 3px;">
            <div style="width: 300px;"><img style="vertical-align: middle;" src="${h.url_for('/static/style/error_small.png')}">&nbsp;<span style="vertical-align: middle;">${error_dict[param.name]}</span></div>
        </div>
        %endif
        %if param.help:
            %if len(param.help) > 120:
                <div id="${help_id}" class="toolParamHelp" style="clear: both;display:none;">
                    ${param.help}
                </div>
            %else:
                <div id="${help_id}" class="toolParamHelp" style="clear: both;">
                    ${param.help}
                </div>
            %endif
        %endif
        <div style="clear: both"></div>
    </div>
</%def>

<div id='ec_button_container'>
    <span class="action-button" id="show_all_tool_body">Expand All</span>
    <span class="action-button" id="hide_all_tool_body">Collapse</span>
</div>

<%
    history = trans.get_history();
%>
<h1>Configure Workflow Run for "${h.to_unicode( history.name )}"</h1>

%if has_upgrade_messages:
<div class="warningmessage">
    Problems were encountered when loading this workflow, likely due to tool
    version changes. Missing parameter values have been replaced with default.
    Please review the parameter values below.
</div>
%endif

%if workflow.annotation:
    <div class="workflow-annotation">${workflow.annotation}</div>
    <hr/>
%endif

<form id="tool_form" name="tool_form" method="POST">
## <input type="hidden" name="workflow_name" value="${h.to_unicode( workflow.name ) | h}" />

<!-- TODO: Implement UI for selecting between product and matched mode
     for batch workflows in multiple inputs are selected for 2 or more
     params.

     1) Delete this line above: $('.multiinput').addClass('disabled');
     2) Allow user to select between product and matched mode.

     If user selected 5 inputs for one param and 5 inputs for another
     in matched mode that will be run the workflow 5 times matching
     each input and in product mode it will run the workflow 25 times
     with every combination of input pairs. If user selects 6 inputs
     for one param and 4 for another, in product mode 24 workflows
     will run and in matched mode the submission will fail.

     In matched mode the inputs are matched from top to bottom
     regardless of the order they are actually select in. This
     behavior is I assume the desired behavior but I have only tested
     it in chrome, care should be taken to test behavior on other
     browsers and augment UI to ensure numbers of inputs matches
     up.
-->
<input type="hidden" name="multiple_input_mode" value="matched" /> <!-- product or matched -->


%if wf_parms:
<div class="metadataForm">
    <div class="metadataFormTitle">Workflow Parameters</div>
    <div class="metadataFormBody">
    %for parm in wf_parms:
        <div class='form-row'><label style='width:100px;'>${parm}<input style="border:2px solid ${wf_parms[parm]};border-left-width:8px;" type="text" class='wf_parm_input ptag_${parm}' name="wf_parm|${parm}" value=""/></label></div>
    %endfor
    </div>
</div>
    <script type="text/javascript">
    // Set the change hooks for workflow parameters.
    $(document).ready(function () {
        $('.wf_parm_input').bind('change keypress keyup', function(event){
            // DBTODO This is probably not reliable.  Ensure we have the right class.
            var new_text = $(this).val();
            if (new_text === ''){
                var tag_id = $(this).attr("class").split(' ')[1].substring(5);
                // Set text properly.
                $('.wfpspan.wf_parm__'+tag_id).text(tag_id);
            }else{
                var tag_id = $(this).attr("class").split(' ')[1].substring(5);
                // Set text properly.
                $('.wfpspan.wf_parm__'+tag_id).text(new_text);
                // Now set the hidden input to the generated text.
                $('.wfpspan.wf_parm__'+tag_id).not('.pja_wfp').each(function(){
                    var new_text = $(this).parent().text();
                    $(this).parent().siblings().children().val(new_text);
                });
            }
        });
    });
    </script>
%endif
%for i, step in enumerate( steps ):
    %if step.type == 'tool' or step.type is None:
      <%
        tool = trans.app.toolbox.get_tool( step.tool_id )
      %>
      <input type="hidden" name="${step.id}|tool_state" value="${step.state.encode( tool, app )}">
      <div class="toolForm">
          <div class="toolFormTitle">
              <span class='title_ul_text'>Step ${int(step.order_index)+1}: ${tool.name}</span>
              %if tool.version:
                  (version ${tool.version})
              %endif
              % if step.annotations:
                <div class="step-annotation">${h.to_unicode( step.annotations[0].annotation )}</div>
              % endif
          </div>
          <div class="toolFormBody">
                ${do_inputs( tool.inputs, step.state.inputs, errors.get( step.id, dict() ), "", step, None, used_accumulator )}
                % if step.post_job_actions:
                    <hr/>
                    <div class='form-row'>
                    % if len(step.post_job_actions) > 1:
                        <label>Actions:</label>
                    % else:
                        <label>Action:</label>
                    % endif
                    <%
                    pja_ss_all = []
                    for pja_ss in [ActionBox.get_short_str(pja) for pja in step.post_job_actions]:
                        for rematch in re.findall('\$\{.+?\}', pja_ss):
                            pja_ss = pja_ss.replace(rematch, '<span style="background-color:%s" class="wfpspan wf_parm__%s pja_wfp">%s</span>' % (wf_parms[rematch[2:-1]], rematch[2:-1], rematch[2:-1]))
                        pja_ss_all.append(pja_ss)
                    %>
                    ${'<br/>'.join(pja_ss_all)}
                    </div>
                % endif
              </div>
          </div>
    %else:
        <% module = step.module %>
        <input type="hidden" name="${step.id}|tool_state" value="${module.encode_runtime_state( t, step.state )}">
        <div class="toolForm">
            <div class="toolFormTitle">
                <span class='title_ul_text'>Step ${int(step.order_index)+1}: ${module.name}</span>
                % if step.annotations:
                  <div class="step-annotation">${step.annotations[0].annotation}</div>
                % endif
            </div>
            <div class="toolFormBody">
                <%
                # Filter possible inputs to data types that are valid for subsequent steps
                type_filter = []
                type_display = None
                for oc in step.output_connections:
                    for ic in oc.input_step.module.get_data_inputs():
                        if 'extensions' in ic and ic['name'] == oc.input_name:
                            type_filter += ic['extensions']
                        if 'display' in ic and ic['name'] == oc.input_name:
                            type_display = ic['display']
                if not type_filter:
                    type_filter = ['data']
                %>
                ${do_inputs( module.get_runtime_inputs(type_filter, type_display), step.state.inputs, errors.get( step.id, dict() ), "", step, None, used_accumulator )}
            </div>
        </div>
    %endif
%endfor

%if missing_tools:
    <div class='errormessage'>
    <strong>This workflow utilizes tools which are unavailable, and cannot be run.  Enable the tools listed below, or <a href="${h.url_for(controller='workflow', action='editor', id=trans.security.encode_id(workflow.id) )}" target="_parent">edit the workflow</a> to correct these errors.</strong><br/>
    <ul>
    %for i, tool in enumerate( missing_tools ):
        <li>${tool}</li>
    %endfor
    </ul>
%else:
    %if history_id is None:
<p id='new_history_p'>
    <input type="checkbox" name='new_history' value="true" id='new_history_cbx'/><label for='new_history_cbx'>Send results to a new history </label>
    <span id="new_history_input">named: <input type='text' name='new_history_name' value='${h.to_unicode( workflow.name )}'/></span>
</p>
    %endif
<input type="submit" class="btn btn-primary" name="run_workflow" value="Run workflow" style="font-size:13pt; margin-left:60px; margin-bottom:30px;" />
</form>
%endif
