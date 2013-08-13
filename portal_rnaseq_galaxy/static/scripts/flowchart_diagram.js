    //rectangleData: 0: rectangle x
    //      1: rectangle y
    //      2: rectangle width
    //      3: rectangle height
    //      4: text line 1
    //      5: text line 1 dx
    //      6: text line 2 dy
    //      7: text line 2
    //      8: text line 2 dx
    //      9: text line 2 dy
    //     10: fill color
    //     11: id
    var rectangleData = [
        [80, 30, 100, 40, "TRIMMING", 13, 25, "", 0, 0, "function none_selected", "trimming"],
        [340, 30, 120, 40, "DEDUPLICATION", 8, 25, "", 0, 0, "function none_selected", "deduplication"],
        [180, 120, 165, 50, "ALIGNMENT", 40, 20, "& MAPPING", 45, 40, "function none_selected", "mapping"],
        [475, 120, 175, 50, "TRANSCRIPT", 45, 20, "ASSEMBLY", 50, 40, "function none_selected", "transcript_assembly"],
        [770, 120, 165, 50, "DIFFERENTIAL", 35, 20, "EXPRESSION ANALYSIS", 10, 40, "function none_selected", "differential_expression_analysis"],
        [20, 200, 130, 50, "READS QUALITY", 15, 20, "CHECK", 40, 40, "info none_selected", "reads_quality_check"],
        [370, 200, 135, 50, "MAPPING QUALITY", 8, 20, "CHECK", 40, 40, "info none_selected", "mapping_quality_check"]
    ];

    //lineData: 0: x1
    //          1: y1
    //          2: x2
    //          3: y2
    //          4: style (solid, dashed)
    //          5: color
    //          6: dot true/false
    //          7: arrowhead true/false
    //          8: text
    var lineData = [
        [10, 145, 180, 145, "solid", "#000000", true, true, "FASTQ"], //to Mapping
        [345, 145, 475, 145, "solid", "#000000", false, true, "BAM"], //to Transcript Assembly
        [650, 145, 770, 145, "solid", "#000000", false, true, "GTF,BED,FPKM..."], //to Differential Analysis
        [1095, 145, 935, 145, "solid", "#000000", true, false, "LOG RATIOS, P-VALUES..."], //
        [85, 145, 85, 200, "dashed", "#999999", true, false, ""], //to Reads Quality Check
        [120, 145, 120, 70, "dashed", "#999999", true, true, ""], //to Trimming
        [140, 70, 140, 145, "dashed", "#999999", false, true, "FASTQ"], //from Trimming
        [390, 145, 390, 70, "dashed", "#999999", true, true, ""], //to Deduplication
        [410, 70, 410, 145, "dashed", "#999999", false, true, "BAM"], //from Deduplication
        [435, 145, 435, 200, "dashed", "#999999", true, false, ""], //to Mapping Quality Check       
    ];

    var flowchart_svg = d3.select("#flowchart_area")
        .append("svg:svg")
        .attr("viewBox", "0 0 1150 300")
        .attr("preserveAspectRatio", "xMinYMin meet")
        .attr("width", "780")
        .attr("height", "200")
        .on("click", function(){flowchartClicked()});

    for(var i = 0; i < rectangleData.length; i++) {
        flowchart_svg.append("svg:rect").attr("id", rectangleData[i][11]).classed("unselected", true);
        flowchart_svg.append("svg:text").attr("class", "rectangle_label");
    }

    for(var i = 0; i < lineData.length; i++) {
        flowchart_svg.append("svg:circle");
        flowchart_svg.append("svg:line");
        flowchart_svg.append("svg:text").attr("class", "line_label");
        flowchart_svg.append("svg:path");
    }

    d3.selectAll("text").attr("pointer-events", "none");
    var rectangles = d3.selectAll("rect");
    rectangles.data(rectangleData);
    var rectangle_labels = d3.selectAll("text.rectangle_label");
    rectangle_labels.data(rectangleData);

    rectangles
        .attr("x", function(d){
            var r = d[0];
            return r;
        })
        .attr("y", function(d){
            var r = d[1];
            return r;
        })
        .attr("width", function(d){
            var r = d[2];
            return r;
        })
        .attr("height", function(d){
            var r = d[3];
            return r;
        })
        .attr("rx", 6)
        .attr("ry", 6)
        .attr("class", function(d){return d[10]})
        .style("stroke", "black")
        .style("stroke-dasharray", "0")
    ;


    rectangle_labels
         .append("tspan")
         .text(function(d){
             var r = d[4];
             return r;
         })
         .attr("x", function(d){
             var r = d[0] + d[5];
             return r;
         })
         .attr("y", function(d){
             var r = d[1] + d[6];
             return r;
         })
         .style("font-size", 13);

    rectangle_labels
         .append("tspan")
         .text(function(d){
             var r = d[7];
             return r;
         })
         .attr("x", function(d){
             var r = d[0] + d[8];
             return r;
         })
         .attr("y", function(d){
             var r = d[1] + d[9];
             return r;
         })
         .style("font-size", 13);

    var paths = d3.selectAll("path");
    paths.data(lineData);
    paths
        .attr("transform", function(d){
            var r = "translate(0,0)"; 
            if(d[7]) {
                r = "translate(" 
                    + d[2] + "," + d[3] + ")";
                if(d[1] > d[3]) {
                    r += " rotate(-90)";
                } else if(d[3] > d[1]) {
                    r += " rotate(90)";
                }
            }
            return r;
        })
        .attr("d",  "M -8 -5 0 0 -8 5 Z")
        .style("fill", function(d){
            var r = d[5];
            return r;
        });

    var line_labels = d3.selectAll("text.line_label");
    line_labels.data(lineData);
    line_labels
        .text(function(d){
            var r = d[8];
            return r;
        })
        .attr("x", function(d){
            var r = Math.min(d[0], d[2]);
            r += 5;
            return r;
        })
        .attr("y", function(d){
            var r = Math.min(d[1], d[3]);
            r -= 3;
            return r;
        })
        .attr("fill", "#999999")
        .attr("style", "font-style:italic;")
        .attr("transform", function(d){
            var r = "";
            if(d[0] == d[2]) {
                r = "rotate(90 "
                  + d[0] + "," + d[1] + ")";
            }
            return r;
        })
        .style("font-size", "12")
        ;

    var dots = d3.selectAll("circle");
    dots.data(lineData);
    dots
        .attr("cx", function(d){
            var r = d[0];
            return r;
        })
        .attr("cy", function(d){
            var r = d[1];
            return r;
        })
        .attr("r", function(d){
            var r = "0px";
            if(d[6]) {
                r = "4px";
            }
            return r;
        })
        .style("fill", function(d){
            var r = d[5];
            return r;
        });

    var lines = d3.selectAll("line");
    lines.data(lineData);
    lines
        .attr("x1", function(d){
            var r = d[0];
            return r;
        })
        .attr("y1", function(d){
            var r = d[1];
            return r;
        })
        .attr("x2", function(d){
            var r = d[2];
            return r;
        })
        .attr("y2", function(d){
            var r = d[3];
            return r;
        })
        .style("stroke", function(d){
            var r = d[5];
            return r;
        })
        .style("stroke-dasharray", function(d){
            var r = "";
            if(d[4] == "dashed") {
                r = "8, 4";
            }
            return r;
        })
        ;

    function selectStep(id) {
        if(id == null) {
            clearSelections();
        } else {
            d3.selectAll("rect")
              .classed("selected", false)
              .classed("unselected", true)
              .classed("none_selected", false);
            if(Object.prototype.toString.call(id) === '[object Array]') {
                for(var i = 0; i < id.length; i++) { 
                    var selected = d3.select("#" + id[i]);
                    selected.classed("unselected", false);
                    selected.classed("selected", true);
                }
            } else {
                var selected = d3.select("#" + id);
                selected.classed("unselected", false);
                selected.classed("selected", true);
            }
        }
    }

    function flowchartClicked() {
        var clickedId = d3.event.srcElement.id;
        stepSelected(clickedId);
    }

