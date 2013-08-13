<%inherit file="/base.mako"/>
<%def name="title()">Configure Launch</%def>

<%def name="stylesheets()">
    ${parent.stylesheets()}
    ${h.css('hanger')}
</%def>



<%def name="javascripts()">

    ${parent.javascripts()}
    ${h.js( "/libs/jquery/jquery", "galaxy.base" )}
    
    <script type="text/javascript">
        $(function() {
            $("#select-multiple").click(function() {
                $("#single-dest-select").val("");
                $("#single-destination").hide();
                $("#multiple-destination").show();
            });
        });
    
        var history_url='${history_url}';

        function refresh_source(){
            var source_contents_url = history_url+'/'+$("#source_history").val()+'/contents';
            var source_data=$("#source_data");
            source_data.html("");
            $.getJSON(source_contents_url, function(response) {
                $.each(response, function(key, val) {
                $('<input type="checkbox" name="source_dataset_ids" id="dataset_'+val.id+'" value="'+val.id+'"/>').appendTo(source_data);
                $('<label for="dataset_'+val.id+'" style="display: inline;font-weight:normal;"> '+val.name+' </label/> <br />').appendTo(source_data);
                });
            });
         }
         
        function refresh_target(){
            var target_data=$("#target_data");
            target_data.html("");
            if($("#single-dest-select").val() != null){
                document.getElementById('current_project').style.color="black";
                document.getElementById('current_project').innerHTML=$("#single-dest-select :selected").text();
                var target_contents_url = history_url+'/'+$("#single-dest-select").val()+'/contents';
                $.getJSON(target_contents_url, function(response) {
                    $.each(response, function(key, val) {
                    $('<label for="dataset_'+val.id+'" style="display: inline;font-weight:normal;"> '+val.name+' </label/> <br />').appendTo(target_data);
                   });
                });
            }
	    else{
	        var  cur_project=document.getElementById('current_project');
		if(cur_project != null){
                	cur_project.style.color="red";
               		cur_project.innerHTML="None Selected";
		}
            }
         }
         function refresh_options(){
            var source_list=$("#source_history");
            var target_list=$("#single-dest-select");
            var first_load = 0;
            if (target_list.val()==undefined) {
                first_load = 1;
            }
            var cur_source=source_list.val();              
            $.getJSON(history_url, function(response) {
                source_list.html("");
                target_list.html("");
                $.each(response, function(key, val) {
                $('<option value="'+val.id+'">'+val.name+'</option>').appendTo(source_list);
                $('<option value="'+val.id+'">'+val.name+'</option>').appendTo(target_list);
                });
                if(cur_source != null){
                    source_list.val(cur_source);
                }
                if(first_load==1){
                    target_list.prop("selectedIndex", -1);
                }
                refresh_source();
                refresh_target();
            });
          }
          $(document).ready(function(){
            refresh_options();
            $(window).keydown(function(event){
                if(event.keyCode == 13) {
                event.preventDefault();
                return false;
                }
            });
          });
        
    </script>
</%def>

<div class="start_info">
	<div class="gauge_div" style="visibility: hidden;">
	<dl>
	<dt>
	    <dd>
		View a <a target="_self" class="reference" href="/static/organisms.html">list of supported genomes</a> from 
		<a class="reference" href="http://www.eupathdb.org/">EuPathDB</a>, 
		<a class="reference" href="http://www.patricbrc.org/">PATRIC</a>, 
		and <a class="reference" href="http://www.vectorbase.org/">VectorBase</a>. 
	    </dd>
	    <br/>
	     <dd>
	     Have a question? <a href="mailto:pathporthelp@vbi.vt.edu" title="Send us email">Contact the Pathogen Portal Team</a>
	     </dd>
	</dt>
	</div>

	<div id="flowchart_area"></div>
</div>

<%def name="flowchart_diagram()">
    <script type="text/javascript" src="${h.url_for('/static/scripts/d3.v2.min.js')}"></script>
    <script type="text/javascript" src="${h.url_for('/static/scripts/flowchart_diagram.js')}"></script>
</%def>

%if retrieval_tag=="fastqc":
        <div class="grid-header" style="text-align:center">
            <h1>Check read quality</h1>
        </div>
        <div id="content" class="analyze_content_area">
        <dl>
        <dt>Purpose:</dt>
        <dd>Read base quality can affect how the reads map to the genome. Different sequencing technologies can have different quality and base-call error profiles. Depending on the quaility of base calls you may wish to trim your read squences.</dd>
        <dt>Required Input:</dt>
        <dd>FastQ files</dd>
        <dt>Output:</dt>
        <dd>An HTML report characterizing the base calling quality of reads represented in the FastQ file.</dd>
        </dl>

        </div>

        ${flowchart_diagram()}
        <script type="text/javascript"> 
            selectStep("reads_quality_check");
        </script>

%endif

%if retrieval_tag=="ebi_sra_main":
        <div class="grid-header" style="text-align:center">
            <h1>Get fastq from ENA/SRA</h1>
        </div>
        <div id="content" class="analyze_content_area">
        <dl>
        <dt>Purpose:</dt>
        <dd>The European Nucleotide Archive has Galaxy integration which supports transferring fastq files to this server. On the subsequent page search for a GEO, ArrayExpress, SRA, or ENA accession; navigate to the Read Files tab for the dataset; and click the 'Fastq files (galaxy)' link to upload the fastq file.</dd>
        </dl>

        </div>
        ${flowchart_diagram()}
%endif

%if retrieval_tag=="cuffdiff_to_genelist":
        <div class="grid-header" style="text-align:center">
            <h1>Create GeneList Summary</h1>
        </div>
        <div id="content" class="analyze_content_area">
        <dl>
        <dt>Purpose:</dt>
        <dd>For each comparison designated during a CuffDiff run a 'gene differential expression testing' file is created (among many others). This tool creates a log2 fold change summary (a GeneList file) using these expression testing files. GeneList files can can be used in PATRIC and other transcriptomic analysis tools.</dd>
        <dt>Required Input:</dt>
        <dd>Gene differential expression testing files from CuffDiff</dd>
        <dt>Output:</dt>
        <dd>A GeneList summary file.</dd>
        </dl>

        </div>

        ${flowchart_diagram()}
        <script type="text/javascript"> 
            selectStep("differential_expression_analysis");
        </script>

%endif

%if retrieval_tag=="sickle":
        <div class="grid-header" style="text-align:center">
            <h1>Trim Read Sequences</h1>
        </div>
        <div id="content" class="analyze_content_area">
        <dl>
        <dt>Purpose:</dt>
        <dd>Read base quality can affect how the reads map to the genome. Different sequencing technologies can have different quality and base-call error profiles. Depending on the quaility of base calls you may wish to trim your read squences. Because many aligners can use the quality information to inform the alignment process this step should be considered optional.</dd>
        <dt>Required Input:</dt>
        <dd>FastQ files</dd>
        <dt>Output:</dt>
        <dd>Trimmed FastQ files.</dd>
        </dl>

        </div>

        ${flowchart_diagram()}
        <script type="text/javascript"> 
            selectStep("trimming");
        </script>
%endif

%if retrieval_tag=="samstat":
        <div class="grid-header" style="text-align:center">
            <h1>Check mapping quality</h1>
        </div>
        <div id="content" class="analyze_content_area">
        <dl>
        <dt>Purpose:</dt>
        <dd>Read base quality and alignment parameters can affect how the reads map to the genome. This tool provides a report on the quality of mapping of sequencing reads to the genome.</dd>
        <dt>Required Input:</dt>
        <dd>BAM or SAM file</dd>
        <dt>Output:</dt>
        <dd>An HTML report characterizing the number and quality of alignments.</dd>
        </dl>

        </div>

        ${flowchart_diagram()}
        <script type="text/javascript"> 
            selectStep("mapping_quality_check");
        </script>
%endif

%if retrieval_tag=="rgPicardMarkDups":
        <div class="grid-header" style="text-align:center">
            <h1>Remove Duplicate Read Pairs</h1>
        </div>
        <div id="content" class="analyze_content_area">
        <dl>
        <dt>Purpose:</dt>
        <dd>When PCR amplification is involved in transcript sequencing, priming at exactly the same place should be uncommon for pairs of reads. Read pairs may be overrepresented if their sequence composition gives them an advantage in the amplification process. This option "collapses" read pairs that have identical coordinates for the first and second read to account for this bias.</dd>
        <dt>Required Input:</dt>
        <dd>BAM files</dd>
        <dt>Output:</dt>
        <dd>BAM files deduplicated</dd>
        </dl>

        </div>

        ${flowchart_diagram()}
        <script type="text/javascript"> 
            selectStep("deduplication");
        </script>
%endif


%if retrieval_tag=="peakcalling_macs14":
        <div class="grid-header" style="text-align:center">
            <h1>ChIP-Seq Align Reads and Call Peaks</h1>
        </div>
        <div id="content" class="analyze_content_area">
        <dl>
        <dt>Purpose:</dt>
        <dd>Identify protein binding sites through regions of increased coverage by providing mapped reads to the MACS software.</dd>
        <dt>Required Input:</dt>
        <dd>BAM files (control and tag)</dd>
        <dt>Output:</dt>
        <dd>BED file (peak chromosome coordinates), interval file (enrichment)</dd>
        </dl>

        </div>

%endif


%if retrieval_tag=="solid2fastq":
        <div class="grid-header" style="text-align:center">
            <h1>Convert FastQ Format</h1>
        </div>
        <div id="content" class="analyze_content_area">
        <dl>
        <dt>Purpose:</dt>
        <dd>To convert FastQ files to FastQ Sanger format.</dd>
        <dt>Additional Information:</dt>
        <dd>If you have SOLiD colorspace reads then you will need to convert to FastQ Sanger before using additional pipeline tools.</dd>
        <dt>Required Input:</dt>
        <dd>FastQ files (Color Space or Old Illumina format)</dd>
        <dt>Output:</dt>
        <dd>FastQ files (Sanger format)</dd>
        </dl>

        </div>

%endif

%if retrieval_tag=="cuffde":
        <div class="grid-header" style="text-align:center">
            <h1>Differential Expression Analysis</h1>
        </div>
        <div id="content" class="analyze_content_area">
        <dl>
        <dt>Purpose:</dt>
        <dd>Deterimine if assembled transcripts are significantly differentially expressed between RNA-Seq Samples.</dd>
        <dt>Additional Information:</dt>
        <dd>In order to represent both novel transcripts and annotated genes, the differential expression tool needs the corresponding GTF and BAM files for each sample that will be compared. Ideally these files will have been generated in the Alignment and Assembly step.</dd>
        <dt>Required Input:</dt>
        <dd>BAM files from mapping reads to the Pathogen Portal provided genome</dd>
        <dd>GTF files generated by cufflinks during the transcript assembly</dd>
        <dt>Output:</dt>
        <dd>Tab delimited files tracking which genes are differentially expressed between sample pairs.</dd>
        </dl>

        </div>

        ${flowchart_diagram()}
        <script type="text/javascript"> 
            selectStep("differential_expression_analysis");
        </script>
%endif


%if retrieval_tag=="cuffcompare":
        <div class="grid-header" style="text-align:center">
            <h1>Compare Transcript Assembly to an Existing Annotation</h1>
        </div>
        <div id="content" class="analyze_content_area">
        <dl>
        <dt>Purpose:</dt>
        <dd>Compare assembled transcripts to an existing annotation. Useful in determining if RNA-Seq data indicates novel features or confirms existing annotations.</dd>
        <dt>Required Input:</dt>
        <dd>GTF files generated by cufflinks during the transcript assembly</dd>
        <dd>The GTF file providing the annotation you wish to compare to</dd>        
        <dt>Output:</dt>
        <dd>Tab delimited files indicating which transcipts correspond to existing annotations.</dd>
        </dl>

        </div>

%endif

%if retrieval_tag=="pipeline":
        <div class="grid-header" style="text-align:center">
            <h1>Align Reads &amp; Assemble Transcripts</h1>
        </div>
        <div id="content" class="analyze_content_area">
        <dl>
        <dt>Purpose:</dt>
        <dd>This procedure will map RNA-Seq reads to one of the provided reference genomes and use this mapping to assemble transcripts, map transcripts to existing annotations, and determine the level of expression. Choose the appropriate option for your organism (Eukaryotic/Prokaryotic) and read type (Paired-end/ Single-end) below.</dd>
        <dt>Required Input:</dt>
        <dd>FastQ files</dd>
        <dt>Output:</dt>
        <dd>Read alignments (BAM Files), tab delimited assembly and expression files for known genes, isoforms, and novel transcripts.</dd>
        </dl>

        </div>

        ${flowchart_diagram()}
        <script type="text/javascript"> 
            selectStep(["mapping", "transcript_assembly"]);
        </script>

%endif


%if retrieval_tag=="bedtools":
        <div class="grid-header" style="text-align:center">
            <h1>BED Tools</h1>
        </div>
        <div id="content" class="analyze_content_area">
        <dl>
        <dt>Purpose:</dt>
        <dd>BEDTools generates BED files which can be used to analyze genome coverage of RNA-Seq reads and for comparing genomic features.</dd>
        <dt>Required Input:</dt>
        <dd>BAM or gtf files</dd>
        <dt>Output:</dt>
        <dd>BED or BEDGraph files</dd>
        </dl>

        </div>

        ${flowchart_diagram()}

%endif

%if retrieval_tag=="aligners":
        <div class="grid-header" style="text-align:center">
            <h1>Align Reads</h1>
        </div>
        <div id="content" class="analyze_content_area">
        <dl>
        <dt>Purpose:</dt>
        <dd>This procedure will map RNA-Seq reads to one of the provided reference genomes or your own genome. Choose the appropriate option for your organism (Eukaryotic/Prokaryotic) below.</dd>
        <dt>Required Input:</dt>
        <dd>FastQ files</dd>
        <dt>Output:</dt>
        <dd>Read alignments (BAM Files)</dd>
        </dl>

        </div>

        ${flowchart_diagram()}
        <script type="text/javascript"> 
            selectStep("mapping");
        </script>

%endif

%if retrieval_tag=="assemblers":
        <div class="grid-header" style="text-align:center">
            <h1>Assemble Transcripts</h1>
        </div>
        <div id="content" class="analyze_content_area">
        <dl>
        <dt>Purpose:</dt>
        <dd>This procedure will assemble transcripts, map transcripts to existing annotations, and determine the level of expression. Choose the appropriate option for your organism (Eukaryotic/Prokaryotic).</dd>
        <dt>Required Input:</dt>
        <dd>Aligned reads (BAM files)</dd>
        <dt>Optional Input:</dt>
        <dd>GTF/GFF annotation file whose replicon/contig IDs match the replicon IDs of your reference genome used during the alignment step.</dd>
        <dt>Output:</dt>
        <dd>Tab delimited assembly and expression files for known genes, isoforms, and novel transcripts.</dd>
        </dl>

        </div>

        ${flowchart_diagram()}
        <script type="text/javascript"> 
            selectStep("transcript_assembly");
        </script>
%endif

%if retrieval_tag=="solid":
        <div class="grid-header" style="text-align:center">
            <h1>Align SOLiD Reads</h1>
        </div>
        <div id="content" class="analyze_content_area">
        <dl>
        <dt>Purpose:</dt>
        <dd>This procedure will map RNA-Seq reads to a genome. Currently for colorspace reads users must provide their own genome to be indexed at runtime. Choose the appropriate option for your organism (Eukaryotic/Prokaryotic) below.</dd>
        <dt>Required Input:</dt>
        <dd>FastQ files</dd>
        <dd>Genome fasta file</dd>
        <dt>Output:</dt>
        <dd>Read alignments (BAM Files)</dd>
        <dt>Additional Information:</dt>
        <dd>Taking advantage of BRC annotations in downstream processing requires that the replicon IDs in the provided genome file match the replicon IDs in the BRC annotations.</dd>
        </dl>

        </div>

%endif

<%
num_w=len(candidate_workflows)
num_t=len(candidate_tools)
hide_projects=False
%>
%if retrieval_tag=="ebi_sra_main":
<%  
    num_t=0
    hide_projects=True
    sra_link=candidate_tools.keys()[0]
%>

<div class="form-row" id="launch_row" align="center" style="width: 100%">
    <a class="ebi_sra_main tool-link fakebutton launchbutton" href="${sra_link}" target="galaxy_main" minsizehint="800">Continue</a>
</div>

%else:
	<form id="history_form" method="post" target="_top">

	%if num_w + num_t > 0:
		<%
		hide_div="block"
		if num_t+num_w==1:
		    hide_div="none"
		%>        
		<div class="hangeranalysis">
		<h3>Select Analysis Type</h3>

		%for name, workflow in candidate_workflows.iteritems():
		    <input type="radio" name="target_workflow_id" value="${trans.security.encode_id(workflow.stored_workflow.id)}">${name}<br>
		%endfor
		%for name, cur_tool in candidate_tools.iteritems():
		    % if not name in candidate_workflows:
			<input type="radio" name="target_tool_id" value="${cur_tool}">${name}<br>
		    % endif
		%endfor
		</div>
	%endif

	%if num_w == 0 and num_t == 0:
		<input type="hidden" name="target_tool_id" id="target_tool_id" value="${retrieval_tag}">
	%endif


	%if error_msg:
	    <p>
		<div class="errormessage">${error_msg}</div>
		<div style="clear: both"></div>
	    </p>
	%endif
	%if done_msg:
	    <p>
		<div class="donemessage">${done_msg}</div>
		<div style="clear: both"></div>
	    </p>
	%endif

	%if not hide_projects:
	<div class="hangerprojects">
	    <div style="float: left; width: 45%">
		    <div class="hangermessage">Select an existing Project or create a new Project to be used during this analysis and populate the Project with the necessary files. Output from this analysis will be saved in the selected Project.<br/>
		    <h3>Currently Selected Project: <label id="current_project"></label></h3>
		    </div> 
		    <div style="clear: both"></div>
	    </div>
	    <div style ="display:block; float:left; height: 112px; width:10%;"></div>
	    <div style="float: left; width: 45%; height: 112px;">
		    <div class="hangermessage" style="border-color: #999999; height: 76px;">Select and copy files from Uploads or existing project(s) to populate your current Project.<br/>
		    </div>
		    <div style="clear: both"></div>
	    </div>
	</div>

	<p>
	<div class="errormessage" id="project_error" style="display: none;"></div>
	</p>
	<p>
	    

	<div class ="hangerprojects">
	    <div style ="display:block; float: left; width: 45%;">
		<div class="toolForm" style="padding: 0px;">
		    <div class="toolFormTitle" style="height: 90px; border: 4px solid rgb(124, 148, 93);">Target Project:<br>
			<h4 style="float: left; width: 40%; padding: 0px; display: inline; font-weight:normal;">Select existing project</h4>
			<h4 style="float: left; width: 20%; padding: 0px; color: #000000;">&mdash; OR &mdash;</h4>
			<button id="new_project_button" style="float: right; margin-right: 5px; margin-bottom: 1px; padding: 2px 4px 2px 4px; display: inline; font-weight:normal;" type="button">Create project</button>
			<div class="form-row" id="source_row" style="display:inline-block; padding: 0px 0px; width: 100%; margin-top: 4px;">
			    <select id="single-dest-select" name="target_history_id" style="float: left; width: 170px; padding: 0px; margin-top: 4px;">
			    </select>
			    <h4 style="float: left; width: 30%; padding: 0px;"></h4>
			    <input type="textbox" id ="new_history_name" name="new_history_name" style=" width: 30%; padding: 0px; float: right; margin-top: 4px; margin-right: 4px">
			</div>
		
		    </div>
		    <div class="toolFormBody">
			<div class="scroll_box" id="target_data">
			</div>
		    </div>
		</div>
	    </div>
	    <div style ="display:block; float:left; width:10%">
		<button id="copy_data" style="float: left; padding-left: 10px; margin-top: 30px; margin-left: 15px; font-size: 18px" type="button" name="do_copy"> &larr; Copy</button>
	    </div>
	    <div style ="display:block; float:left; width:45%">
		<div class="toolForm" style="padding: 0px;">
		    <div class="toolFormTitle" style="height: 90px; border: 4px solid #999999;">Source Project:<br />
			<h4 style="float: left; width: 40%; padding: 0px; display: inline; font-weight:normal;">Select source</h4><br />
			<select id="source_history" name="source_history" refresh_on_change="false" style="font-weight: normal; width: 170px;">
			</select>
		    </div>
		    <div class="toolFormBody">
			<div class="scroll_box" id="source_data">
			</div>
		    </div>
		</div>
		    <div style="clear: both"></div>
	    </div>
	</div>

	<div class="form-row" id="launch_row" align="center" style="width: 100%">
		    <button id="launch_button" class="launchbutton"  type="submit">Continue</button>
		    
	</div>
	%endif
	</form>
%endif #end if block for not a sra_era_main

    ${h.js( 'galaxy.panels' )}
    <script type="text/javascript">

        function set_target(target_val){
            if (typeof target_val == "undefined") {
                var targets=$("#single-dest-select option");
                $("#single-dest-select").val(targets[0].value);
            }
            else {
                $("#single-dest-select").val(target_val);
             }
        }
        function set_source(source_val){
            if (typeof source_val == "undefined") {
                var sources=$("#source_history option");
                $("source_history").val(sources[0].value);
            }
            else {
                $("#source_history").val(source_val);
             }
        }
        $("#source_history").change(function() { refresh_source(); });
        $("#single-dest-select").change(function() { 
            refresh_target();
        });
        
        $("#history_form").submit(function(e){
            if ($("#single-dest-select").val() == null){
                return false;
            }
            if ( parent.frames && parent.frames.galaxy_history ) {
                parent.frames.galaxy_history.location.href="${h.url_for( controller='root', action='history')}";
                parent.frames.galaxy_history.force_right_panel('show');
            }
            return true;
        });
        
        $("#copy_data").click(function(){
            //var checked_data=$('#source_data :checked');
            $('#source_data :checked').each(function() {
                var copy_url=history_url+'/'+$("#single-dest-select").val()+'/contents';
                $.post(copy_url, {"from_hda_id": $(this).val()}, function(data){}, "application/json");
            });
            $(document).ajaxStop(function() {
                refresh_options();
                $(this).unbind('ajaxStop');
            });
        });
        
        $("#new_project_button").click(function() { 
            var new_project_name=$("#new_history_name").val();
            if (new_project_name == ""){
                var project_error =$("#project_error");
                project_error.html("You must specify a project name.");
                project_error.css("display","inline-block");
                return false;
                }
            var project_error =$("#project_error");
            project_error.css("display","none");
            project_error.html("");
            
            $.post(history_url, {name: new_project_name}, function(data) {
                }, "application/json");
            $(document).ajaxStop(function() {
                refresh_options();
                $(this).unbind('ajaxStop');
            });
            $("#new_history_name").val("");
        });
        
    </script>
</p>
