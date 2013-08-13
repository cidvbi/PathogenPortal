Ext.Loader.setConfig({enabled: true});

Ext.Loader.setPath('Ext.ux', '.');

Ext.require([
    'Ext.grid.*',
    'Ext.data.*',
    'Ext.util.*',
    'Ext.tip.QuickTipManager',
    'Ext.ux.LiveSearchGridPanel'
]);

var gridPanel = null;


Ext.onReady(function() {    
    Ext.QuickTips.init();
        
    // sample static data for the store
    var genomeData = [
        ["Acaryochloris sp. CCMEE 5410", "cellular organisms; Bacteria; Cyanobacteria; Chroococcales; Acaryochloris"],
        ["Acetobacter aceti NBRC 14818", "cellular organisms; Bacteria; Proteobacteria; Alphaproteobacteria; Rhodospirillales; Acetobacteraceae; Acetobacter; Acetobacter subgen. Acetobacter; Acetobacter aceti"],
        ["Acidithiobacillus sp. GGI-221", "cellular organisms; Bacteria; Proteobacteria; Gammaproteobacteria; Acidithiobacillales; Acidithiobacillaceae; Acidithiobacillus"],
        ["Acidovorax avenae subsp. avenae RS-1", "cellular organisms; Bacteria; Proteobacteria; Betaproteobacteria; Burkholderiales; Comamonadaceae; Acidovorax; Acidovorax avenae; Acidovorax avenae subsp. avenae"],
        ["Acidovorax radicis N35", "cellular organisms; Bacteria; Proteobacteria; Betaproteobacteria; Burkholderiales; Comamonadaceae; Acidovorax; Acidovorax radicis"],
        ["Acidovorax radicis N35v", "cellular organisms; Bacteria; Proteobacteria; Betaproteobacteria; Burkholderiales; Comamonadaceae; Acidovorax; Acidovorax radicis"],
        ["Acinetobacter baumannii 3909", "cellular organisms; Bacteria; Proteobacteria; Gammaproteobacteria; Pseudomonadales; Moraxellaceae; Acinetobacter; Acinetobacter calcoaceticus/baumannii complex; Acinetobacter baumannii"],
        ["Acinetobacter baumannii 3990", "cellular organisms; Bacteria; Proteobacteria; Gammaproteobacteria; Pseudomonadales; Moraxellaceae; Acinetobacter; Acinetobacter calcoaceticus/baumannii complex; Acinetobacter baumannii"],
        ["Acinetobacter baumannii 4190", "cellular organisms; Bacteria; Proteobacteria; Gammaproteobacteria; Pseudomonadales; Moraxellaceae; Acinetobacter; Acinetobacter calcoaceticus/baumannii complex; Acinetobacter baumannii"],
        ["Acinetobacter baumannii A118", "cellular organisms; Bacteria; Proteobacteria; Gammaproteobacteria; Pseudomonadales; Moraxellaceae; Acinetobacter; Acinetobacter calcoaceticus/baumannii complex; Acinetobacter baumannii"],
        ["Acinetobacter baumannii AB210", "cellular organisms; Bacteria; Proteobacteria; Gammaproteobacteria; Pseudomonadales; Moraxellaceae; Acinetobacter; Acinetobacter calcoaceticus/baumannii complex; Acinetobacter baumannii"],
        ["Acinetobacter baumannii D1279779", "cellular organisms; Bacteria; Proteobacteria; Gammaproteobacteria; Pseudomonadales; Moraxellaceae; Acinetobacter; Acinetobacter calcoaceticus/baumannii complex; Acinetobacter baumannii"],
        ["Acinetobacter baumannii MDR-TJ", "cellular organisms; Bacteria; Proteobacteria; Gammaproteobacteria; Pseudomonadales; Moraxellaceae; Acinetobacter; Acinetobacter calcoaceticus/baumannii complex; Acinetobacter baumannii"],
        ["Acinetobacter baumannii Naval-18", "cellular organisms; Bacteria; Proteobacteria; Gammaproteobacteria; Pseudomonadales; Moraxellaceae; Acinetobacter; Acinetobacter calcoaceticus/baumannii complex; Acinetobacter baumannii"],
        ["Acinetobacter baumannii Naval-81", "cellular organisms; Bacteria; Proteobacteria; Gammaproteobacteria; Pseudomonadales; Moraxellaceae; Acinetobacter; Acinetobacter calcoaceticus/baumannii complex; Acinetobacter baumannii"],
        ["Acinetobacter baumannii OIFC032", "cellular organisms; Bacteria; Proteobacteria; Gammaproteobacteria; Pseudomonadales; Moraxellaceae; Acinetobacter; Acinetobacter calcoaceticus/baumannii complex; Acinetobacter baumannii"]
    ];
    
    // create the data store
    var genomeStore = Ext.create('Ext.data.ArrayStore', {
        fields: [
           {name: 'genome'},
           {name: 'lineage'}
        ],
        data: genomeData
    });

    // create the Grid, see Ext.
    gridPanel = Ext.create('Ext.ux.LiveSearchGridPanel', {
        store: genomeStore,
        columnLines: true,
        columns: [
            {
                text     : 'Genome',
                flex     : 1,
                sortable : true, 
                dataIndex: 'genome',
                hidden   : false
            },
            {
                text     : 'Lineage',
                flex     : 1,
                sortable : true,
                dataIndex: 'lineage',
                hidden   : true
            }
        ],
        height: 380,
        width: 450,
        title: 'Select Reference Genome',
        renderTo: 'grid-example',
        defaultStatusText: '',
        processEvent: function(type, view, cell, recordIndex, cellIndex, e) {
                          if(type == 'click') {
                              //console.log("genomeStore.getAt(recordIndex).data.genome: " + genomeStore.getAt(recordIndex).data.genome);
                              //console.log("genomeStore.getAt(recordIndex).data.lineage: " + genomeStore.getAt(recordIndex).data.lineage);
                              var message = "Selected Reference Genome: <b>" + genomeStore.getAt(recordIndex).data.genome + "</b>";
                              document.getElementById("show_selected_div").innerHTML = message;;
                          }
                      },
        viewConfig: {
            stripeRows: true
        }
    });

function listProperties(obj) {
   var propList = "";
   for(var propName in obj) {
      if(typeof(obj[propName]) != "undefined") {
         propList += (propName + ", ");
      }
   }
   console.log(propList);
}
});
