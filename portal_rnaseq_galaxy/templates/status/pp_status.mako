<%inherit file="/root/index.mako"/>
<%def name="init()">
<%
    self.has_left_panel=False
    self.has_right_panel=True
    self.message_box_visible=False
%>
</%def>

<%def name="center_panel()">
    <iframe name="galaxy_main" id="galaxy_main" src="${h.url_for( controller='history', action='list' )}" frameborder="0" style="position: absolute; margin: 0; border: 0 none; height: 100%; width: 100%;"> </iframe>
</%def>
