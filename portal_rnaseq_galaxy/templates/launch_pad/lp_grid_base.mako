<%inherit file="/grid_base.mako"/>


<%def name="javascripts()">
    ${parent.javascripts()}
</%def>


<%def name="grid_title()">
    <div class="toolFormTitle"><h2>${grid.title}</h2></div>
</%def>

## Because body() is special and always exists even if not explicitly defined,
## it's not possible to override body() in the topmost template in the chain.
## Because of this, override grid_body() instead.
<%def name="grid_body( grid )">
    ${self.make_grid( grid )}
</%def>

## Overwrite grid render so it can be embedded in another form.
<%def name="render_grid_table( grid, show_item_checkboxes=True)">
    <%
        # Set flag to indicate whether grid has operations that operate on multiple items.
        multiple_item_ops_exist = False
        for operation in grid.operations:
            if operation.allow_multiple:
                multiple_item_ops_exist = True
                
        # Show checkboxes if flag is set or if multiple item ops exist.
        if show_item_checkboxes or multiple_item_ops_exist:
            show_item_checkboxes = True
    %>
    
    <table id="grid-table" class="grid">
        <thead id="grid-table-header">
            <tr>
                %if show_item_checkboxes:
                    <th>
                        %if query.count() > 0:
                            <input type="checkbox" id="check_all" name=select_all_checkbox value="true" onclick='check_all_items(1);'><input type="hidden" name=select_all_checkbox value="true">
                        %endif
                    </th>
                %endif
                %for column in grid.columns:
                    %if column.visible:
                        <%
                            href = ""
                            extra = ""
                            if column.sortable:
                                if sort_key.endswith(column.key):
                                    if not sort_key.startswith("-"):
                                        href = url( sort=( "-" + column.key ) )
                                        extra = "&darr;"
                                    else:
                                        href = url( sort=( column.key ) )
                                        extra = "&uarr;"
                                else:
                                    href = url( sort=column.key )
                        %>
                        <th\
                        id="${column.key}-header"
                        %if column.ncells > 1:
                            colspan="${column.ncells}"
                        %endif
                        >
                            %if href:
                                <a href="${href}" class="sort-link" sort_key="${column.key}">${column.label}</a>
                            %else:
                                ${column.label}
                            %endif
                            <span class="sort-arrow">${extra}</span>
                        </th>
                    %endif
                %endfor
                <th></th>
            </tr>
        </thead>
        <tbody id="grid-table-body">
            ${parent.render_grid_table_body_contents( grid, show_item_checkboxes )}
        </tbody>
        <tfoot id="grid-table-footer">
            ${parent.render_grid_table_footer_contents( grid, show_item_checkboxes )}
        </tfoot>
    </table>

</%def>


## Render grid table body contents.
<%def name="render_grid_table_body_contents(grid, show_item_checkboxes=False)">
        <% num_rows_rendered = 0 %>
        %if query.count() == 0:
            ## No results.
            <tr><td colspan="100"><em>No Items</em></td></tr>
            <% num_rows_rendered = 1 %>
        %endif
        %for i, item in enumerate( query ):
            <% encoded_id = trans.security.encode_id( item.id ) %>
            <tr \
            %if current_item == item:
                class="current" \
            %endif
            > 
                ## Item selection column
                %if show_item_checkboxes:
                    <td style="width: 1.5em;">
                        <input type="checkbox" name="id" value="${encoded_id}" id="${encoded_id}" class="grid-row-select-checkbox" />
                    </td>
                %endif
                ## Data columns
                %for column in grid.columns:
                    %if column.visible:
                        <%
                            # Nowrap
                            nowrap = ""
                            if column.nowrap:
                              nowrap = 'style="white-space:nowrap;"'
                            # Link
                            link = column.get_link( trans, grid, item )
                            if link:
                                href = url( **link )
                            else:
                                href = None
                            # Value (coerced to list so we can loop)
                            value = column.get_value( trans, grid, item )
                            if column.ncells == 1:
                                value = [ value ]
                        %>
                        %for cellnum, v in enumerate( value ):
                            <%
                                id = ""
                                # Handle non-ascii chars.
                                if isinstance(v, str):
                                    v = unicode(v, 'utf-8')

                            %>
                            <td ${nowrap}>
                            %if href:
                                %if len(grid.operations) != 0:
                                <div id="${id}" class="${cls}" style="float: left;">
                                %endif
                                    <a class="label" href="${href}">${v}</a>
                                %if len(grid.operations) != 0:
                                </div>
                                %endif
                            %else:
                                <div id="${id}" class="${cls}"><label id="${column.label_id_prefix}${encoded_id}" for="${encoded_id}">${v}</label></div>
                            %endif
                            </td>
                        %endfor
                    %endif
                %endfor
                ## Actions column
                <td>
                    <div popupmenu="grid-${i}-popup">
                        %for operation in grid.operations:
                            %if operation.allowed( item ) and operation.allow_popup:
                                <%
                                target = ""
                                if operation.target:
                                    target = "target='" + operation.target + "'"
                                %>
                                %if operation.confirm:
                                    <a class="action-button" ${target} confirm="${operation.confirm}" href="${ url( **operation.get_url_args( item ) ) }">${operation.label}</a>
                                %else:
                                    <a class="action-button" ${target} href="${ url( **operation.get_url_args( item ) ) }">${operation.label}</a>
                                %endif  
                            %endif
                        %endfor
                    </div>
                </td>
            </tr>
            <% num_rows_rendered += 1 %>
        %endfor
        ## Dummy rows to prevent table for moving too much.
        ##%if grid.use_paging:
        ##    %for i in range( num_rows_rendered , grid.num_rows_per_page ):
        ##        <tr><td colspan="1000">  </td></tr>
        ##    %endfor
        ##%endif
</%def>
