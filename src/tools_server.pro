TEMPLATE = subdirs

#Library targets and depdendencies
SUBDIRS = cppCORE \
        cppNGS \
        cppNGSD

SUBDIRS += GSvarServer

SUBDIRS += GSvarServer-TEST
GSvarServer-TEST.depends += GSvarServer

GSvarServer.depends = cppCORE cppNGSD

