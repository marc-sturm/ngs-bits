TEMPLATE = subdirs

#Library targets and depdendencies
SUBDIRS = cppCORE \
        cppNGS \
        cppNGSD

SUBDIRS += GSvarServer
GSvarServer.depends = cppCORE cppNGSD

