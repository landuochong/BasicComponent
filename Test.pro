TEMPLATE = subdirs

SUBDIRS += \
    BasicComponent.pro \
    third_party/abseil-cpp.pro \
    app

BasicComponent.depends = abseil-cpp
app.depends = BasicComponent
