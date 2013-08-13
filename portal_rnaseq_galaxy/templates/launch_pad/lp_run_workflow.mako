<%inherit file="/workflow/run.mako"/>
<%namespace name="lp_search_select" file="/launch_pad/lp_search_select.mako"/>

<%def name="javascripts()">
    ${parent.javascripts()}
</%def>

<%def name="stylesheets()">
    ${parent.stylesheets()}
</%def>

<%
    ## launch_pad display types supported
    display_elements={}
    display_index={'search_select':lp_search_select}
%>

<%def name="register_display( step_id, field_parameter )">
    <%
        display_elements[step_id]=display_type
    %>
</%def>

<%def name="expand_js( )">
    ##get the types of displays for this workflow
    <% display_types = set(display_elements.values()) %>
    ##Extjs section
    ##expand extjs require
    %if len(display_types):
        ${extjs_requirements(display_types)}
    %endif
    ##expand extjs on ready section

</%def>

<%def name="extjs_requirments(display_types)">
    ##get all the extjs_requirements from all the different display types and create the extjs require sectoin
    ##expects unique list of display types
    <%
        display_namespaces=set([display_index[k] for k in display_types if k in display_index])
        extjs_includes=set()
    %>
    %for n in display_namespaces:
        ${n.extjs_require()}
        <%
            extjs_includes.add(cur_req)
        %>
    %endfor
    Ext.require(${list(extjs_includes)});
    <%
        assert False
    %>
</%def>    
            
<%def name="do_inputs( inputs, values, errors, prefix, step, other_values = None, already_used = None )">
  %if other_values is None:
      <% other_values = values %>
  %endif
  %for input_index, input in enumerate( inputs.itervalues() ):
    %if input.type == "repeat":
      <div class="repeat-group">
          <div class="form-title-row"><b>${input.title_plural}</b></div>
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
  ${expand_js()}
</%def>
            
<%def name="row_for_param( param, value, other_values, error_dict, prefix, step, already_used )">
    ## -- ${param.name} -- ${step.state.inputs} --
    %if error_dict.has_key( param.name ):
        <% cls = "form-row form-row-error" %>
    %else:
        <% cls = "form-row" %>
    %endif
    <div class="${cls}">
        <label>${param.get_label()}</label>
        <div>
            %if isinstance( param, DataToolParameter ):
                %if ( prefix + param.name ) in step.input_connections_by_name:
                    <%
                        conn = step.input_connections_by_name[ prefix + param.name ]
                    %>
                    Output dataset '${conn.output_name}' from step ${int(conn.output_step.order_index)+1}
                %else:
                    ## FIXME: Initialize in the controller
                    <%
                    if value is None:
                        value = other_values[ param.name ] = param.get_initial_value_from_history_prevent_repeats( t, other_values, already_used )
                        if not enable_unique_defaults:
                            del already_used[:]
                    %>
                    %if isinstance( param, ShellSelectToolParameter ):
                        ##if this is a Shell Parameter then expand the javascript based on the display type and create unique variables based on the step
                        <%
                            data_string = param.get_data_string()
                            register_display(param.display, str(step.id) + "|" + prefix)
                        %>
                    %endif
                    ${param.get_html_field( t, value, other_values ).get_html( str(step.id) + "|" + prefix )}
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
                ##Check for shell instance of parameters HACK: this should be a more generic base type for Shell Parameters
                %if isinstance( param, ShellSelectToolParameter ):
                    ##if this is a Shell Parameter then expand the javascript based on the display type and create unique variables based on the step
                    <%
                        data_string = param.get_data_string()
                        register_display(param.display, str(step.id) + "|" + prefix)
                    %>
                %endif   
                ${param.get_html_field( t, value, other_values ).get_html( str(step.id) + "|" + prefix )}
                <input type="hidden" name="${step.id}|__runtime__${prefix}${param.name}" value="true" />
            %else:
                %if isinstance( param, ShellSelectToolParameter ):
                    ##if this is a Shell Parameter then expand the javascript based on the display type and create unique variables based on the step
                    <%
                        data_string = param.get_data_string()
                        register_display(param.display, str(step.id) + "|" + prefix)
                    %>
                %endif                
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
        <div style="clear: both"></div>
    </div>
</%def>