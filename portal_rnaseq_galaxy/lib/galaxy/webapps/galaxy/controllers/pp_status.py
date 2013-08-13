from galaxy.webapps.galaxy.controllers.history import *

##This Controller is just for running the pathogen portal status page 
##Right now it just uses HistoryController, and its list function
##
class StatusController( HistoryController ):
    @web.expose
    def index( self, trans ):
        return trans.fill_template( "/status/pp_status.mako" )
