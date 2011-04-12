cmake_minimum_required(VERSION 2.4)
project (IcqJPlusMod)
include_directories(
	"../../include"
	"../miranda-im/miranda/include"
	"./"
)

set(CMAKE_C_FLAGS "-mdll -march=i686 -msse -O2 -pipe -w")
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "-Wl,-O1 -s")
set(CMAKE_INCLUDE_PATH "/usr/i686-pc-mingw32/usr/include")
set(CMAKE_LIBRARY_PATH "/usr/i686-pc-mingw32/usr/lib/")

set(CMAKE_AR "i686-pc-mingw32-ar")
set(CMAKE_C_COMPILER "i686-pc-mingw32-gcc")
set(CMAKE_LINKER "i686-pc-mingw32-ld")
set(CMAKE_CXX_COMPILER "i686-pc-mingw32-g++")
set(CMAKE_RANLIB "i686-pc-mingw32-ranlib")
set(CMAKE_STRIP "i686-pc-mingw32-strip")



ADD_DEFINITIONS(-DBUILD_DLL)
ADD_DEFINITIONS(-D_UNICODE)
ADD_DEFINITIONS(-DUNICODE)


set(icq_SRCS
	./icq_filerequests.c
	./fam_03buddy.c
	./icq_xtraz.c
	./icq_direct.c
	./icq_avatar.c
	./icq_filetransfer.c
	./log.c
	./chan_04close.c
	./icqosc_svcs.c
	./icq_srv_list.c
	./icq_popups.c
	./stdpackets.c
	./icq_opts.c
	./icq_db.c
	./fam_0bstatus.c
	./capabilities.c
	./icqoscar.c
	./chan_05ping.c
	./iconlib.c
	./chan_01login.c
	./fam_0alookup.c
	./icq_fieldnames.c
	./icq_directmsg.c
	./i18n.c
	./UI/askauthentication.c
	./UI/userinfotab.c
	./UI/loginpassword.c
	./fam_09bos.c
	./icq_servlist.c
	./icq_qipstatus.c
	./icq_infoupdate.c
	./utilities.c
	./oscar_filetransfer.c
	./fam_01service.c
	./fam_13servclist.c
	./icq_server.c
	./icq_firstrun.c
	./init.c
	./icq_advsearch.c
	./fam_04message.c
	./icq_clients.c
	./icq_uploadui.c
	./fam_17signon.c
	./chan_03error.c
	./fam_02location.c
	./isee.c
	./icq_xstatus.c
	./icq_reguin.c
	./fam_15icqserver.c
	./changeinfo/editstring.c
	./changeinfo/main.c
	./changeinfo/upload.c
	./changeinfo/dlgproc.c
	./changeinfo/db.c
	./changeinfo/editlist.c
	./changeinfo/constants.c
	./chan_02data.c
	./icq_caps_list.c
	./cookies.c
	./tlv.c
	./icq_tzers.c
	./icq_http.c
	./icq_packet.c
	./directpackets.c
	./icq_rates.c
)
add_executable(icq.dll ${icq_SRCS})
target_link_libraries(icq.dll -lcomctl32 -lws2_32 -lgdi32 -lcomdlg32 -loleaut32)
get_property(icqpath TARGET icq.dll PROPERTY LOCATION)
add_custom_command(
	TARGET icq.dll POSTBULD
	COMMAND upx -9 --compress-icons=0 ${icqpath}
	COMMENT "Compressing library"
)
