//general functions Extjs: quickTips, listProperties

<%def name="extjs_requires()">
    ##set the necessary extjs elements
    <%
        return ['Ext.data.*','Ext.grid.*', 'Ext.util.*', 'Ext.tip.QuickTipManager', 'Ext.ux.LiveSearchGridPanel']
    %>
</%def>



<%def name="general_functions()">
    ## the names of the mako functions to expand that each contain a javascript function
    <%    
        return []
    %>
</%def>

<%def name="specific_functions(id)">
    var ${"gridPanel_"+id.replace('|','_')} = null;
</%def>


<%def name="general_onready_functions()">
   <%    
        return ['quickTips', 'listProperties', 'SearchSelectModel']
    %>
</%def>

<%def name="specific_onready_functions(id, data_store)">
    <%
        search_select_div=id+"|search_select"
        selected_div=id+"|selected"
        gridStore_id="gridStore_"+id.replace('|','_')
        gridData_id="gridData_"+id.replace('|','_')
        gridPanel_id="gridPanel_"+id.replace('|','_')
    %>
    //var ${gridData_id} = ${data_store};
     // create the data store
   
    var ${gridStore_id} = Ext.create('Ext.data.Store', {
        model: 'SearchSelectModel',
        data: ${data_store}
    });
    
    // create the Grid, see Ext.
    ${gridPanel_id} = Ext.create('Ext.ux.LiveSearchGridPanel', {
        store: ${gridStore_id},
        columnLines: true,
        enableCtxMenu: false,
        columns: [
            {
                text     : 'Available',
                flex     : 1,
                sortable : false, 
                dataIndex: 'name',
                hidden   : false,
                searchable: true
            },
            {
                text     : 'Value',
                flex     : 1,
                sortable : false,
                dataIndex: 'value',
                hidden   : true,
                searchable: false
            }
        ],
        height: 380,
        width: 450,
        title: 'Select Available',
        renderTo: '${search_select_div}',
        defaultStatusText: '',
        processEvent: function(type, view, cell, recordIndex, cellIndex, e) {
                          if(type=='deselectAll'){
                              var message = "Nothing selected";
                              document.getElementById("${selected_div}").innerHTML = message;
                              document.getElementById("${id}").value="";
                          }
                          else if(type == 'click' || type == 'selected') {
                              var cur_selected=${gridStore_id}.getAt(recordIndex).data;
                              var message = "Selected: <b>" + cur_selected.name + "</b>";
                              document.getElementById("${selected_div}").innerHTML = message;
                              document.getElementById("${id}").value=cur_selected.value;
                          }
                      },
        viewConfig: {
            stripeRows: true
        }
    });
</%def>



<%def name="listProperties()">
    function listProperties(obj) {
      var propList = "";
      for(var propName in obj) {
          if(typeof(obj[propName]) != "undefined") {
            propList += (propName + ", ");
          }
      }
      console.log(propList);
    }
</%def>

<%def name="SearchSelectModel()">
    Ext.define('SearchSelectModel', {
        extend: 'Ext.data.Model',
        fields: [
            {name: 'name',    type: 'string'},
            {name: 'value',   type: 'string'}
        ]
    }); 
</%def>

<%def name="quickTips()">
    Ext.QuickTips.init();
</%def>