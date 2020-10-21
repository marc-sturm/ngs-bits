TEMPLATE = subdirs

#Library targets and depdendencies
SUBDIRS = cppCORE \
        cppNGSD

SUBDIRS += GSvarServer
GSvarServer.depends = cppCORE

