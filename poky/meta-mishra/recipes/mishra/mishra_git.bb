inherit autotools-brokensep module qperf
DESCRIPTION = "mishra Driver"
LICENSE = "ISC"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/${LICENSE};md5=f3b90e78ea0cffb20bf5cca7947a896d"

PR = "${@base_conditional('PRODUCT', 'psm', 'r0-psm', 'r0', d)}"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://mishra-driver/ "
#SRC_URI = "file://kernel/msm-3.18/tools/testing/selftests/ "

S = "${WORKDIR}/mishra-driver"
#S = "${WORKDIR}/kernel/msm-3.18/tools/testing/selftests"

do_install() {
#    module_do_install
}
