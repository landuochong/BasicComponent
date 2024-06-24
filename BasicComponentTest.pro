TEMPLATE = subdirs

SUBDIRS += \
    BasicComponent.pro \
    third_party/abseil-cpp.pro \
    app/app.pro

BasicComponent.depends = abseil-cpp
app.depends = BasicComponent