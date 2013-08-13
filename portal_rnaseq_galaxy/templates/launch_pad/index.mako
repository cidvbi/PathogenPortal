<%inherit file="/root/index.mako"/>
<%def name="init()">
<%
    self.has_left_panel=False
    self.has_right_panel=True
    self.message_box_visible=False
%>
</%def>

<%def name="javascripts()">
    ${parent.javascripts()}
</%def>
<%def name="late_javascripts()">
    ${parent.late_javascripts()}
    <script type="text/javascript">
        if ( parent.force_right_panel ) {
            parent.force_right_panel( 'hide' );
        }
    </script>
</%def>

<%def name="center_panel()">
    <iframe name="galaxy_main" id="galaxy_main" src="${h.url_for( controller='launch_pad', action='task_menu',)}" frameborder="0" style="position: absolute; margin: 0; border: 0 none; height: 100%; width: 100%;"> 
    </iframe>
</%def>
