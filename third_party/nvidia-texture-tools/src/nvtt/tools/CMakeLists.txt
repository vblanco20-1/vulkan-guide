
ADD_EXECUTABLE(nvcompress compress.cpp cmdline.h)
TARGET_LINK_LIBRARIES(nvcompress nvcore nvimage nvtt bc6h nvmath)

ADD_EXECUTABLE(nvdecompress decompress.cpp cmdline.h)
TARGET_LINK_LIBRARIES(nvdecompress nvcore nvimage nvtt bc6h nvmath)

ADD_EXECUTABLE(nvddsinfo ddsinfo.cpp cmdline.h)
TARGET_LINK_LIBRARIES(nvddsinfo nvcore nvimage nvtt bc6h nvmath)

ADD_EXECUTABLE(nvimgdiff imgdiff.cpp cmdline.h)
TARGET_LINK_LIBRARIES(nvimgdiff nvcore nvtt bc6h nvmath)

ADD_EXECUTABLE(nvassemble assemble.cpp cmdline.h)
TARGET_LINK_LIBRARIES(nvassemble nvcore nvimage nvtt bc6h nvmath)

ADD_EXECUTABLE(nvzoom resize.cpp cmdline.h)
TARGET_LINK_LIBRARIES(nvzoom nvcore nvimage nvtt bc6h nvmath)

SET(TOOLS nvcompress nvdecompress nvddsinfo nvimgdiff nvassemble nvzoom)

IF(GLEW_FOUND AND GLUT_FOUND AND OPENGL_FOUND)
    INCLUDE_DIRECTORIES(${GLEW_INCLUDE_PATH} ${GLUT_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})
    ADD_EXECUTABLE(nvddsview ddsview.cpp cmdline.h)
    TARGET_LINK_LIBRARIES(nvddsview nvcore nvimage ${GLEW_LIBRARY} ${GLUT_LIBRARY} ${OPENGL_LIBRARY})
    SET(TOOLS ${TOOLS} nvddsview)
ENDIF(GLEW_FOUND AND GLUT_FOUND AND OPENGL_FOUND)


ADD_EXECUTABLE(nv-gnome-thumbnailer thumbnailer.cpp cmdline.h)
TARGET_LINK_LIBRARIES(nv-gnome-thumbnailer nvcore nvtt nvimage bc6h nvmath)

SET(TOOLS ${TOOLS} nv-gnome-thumbnailer)

INSTALL(TARGETS ${TOOLS} DESTINATION bin)
 
 # Use gconftool-2 to install gnome thumbnailer
FIND_PROGRAM(GCONFTOOL2 gconftool-2)

IF(GCONFTOOL2)
	CONFIGURE_FILE(nvtt-thumbnailer.schema.in ${CMAKE_CURRENT_BINARY_DIR}/nvtt-thumbnailer.schema)

 	INSTALL(CODE "MESSAGE(STATUS \"Installing thumbnailer schema\")")
 	#gconftool-2 --get-default-source
 	INSTALL(CODE "EXECUTE_PROCESS(COMMAND ${GCONFTOOL2} --get-default-source OUTPUT_VARIABLE GCONF_CONFIG_SOURCE OUTPUT_STRIP_TRAILING_WHITESPACE)")
 	INSTALL(CODE "set(ENV{GCONF_CONFIG_SOURCE} \"\${GCONF_CONFIG_SOURCE}\")")
 	INSTALL(CODE "EXECUTE_PROCESS(COMMAND ${GCONFTOOL2} --makefile-install-rule ${CMAKE_CURRENT_BINARY_DIR}/nvtt-thumbnailer.schema)")
ENDIF(GCONFTOOL2)

# UI tools
#FIND_PACKAGE(Qt4)
IF(QT4_FOUND)
	SET(QT_USE_QTOPENGL TRUE)
	INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
	
	SET(SRCS
                compressdialog.h
                compressdialog.cpp)

	SET(LIBS
		nvtt
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
		${QT_QTOPENGL_LIBRARY})

        QT4_WRAP_UI(UICS compressdialog.ui)
        QT4_WRAP_CPP(MOCS compressdialog.h)
	#QT4_ADD_RESOURCES(RCCS ui/configdialog.rc)

	#ADD_EXECUTABLE(nvtt-diag MACOSX_BUNDLE ${SRCS} ${UICS} ${MOCS})
	#TARGET_LINK_LIBRARIES(nvtt-diag ${LIBS})

ENDIF(QT4_FOUND)

