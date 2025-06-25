#! /bin/bash
#
# unset all
unset CRYSTALEYE_SYS
unset CRYSTALEYE_DATA
unset CRYSTALEYE_MACRO
#
export CRYSTALEYE_SYS=$HOME/Work/CrystalEye/Source/CEInstall
#
# Set STL data
export CRYSTALEYE_DATA=$CRYSTALEYE_SYS/data/
#
# Set macro dir
export CRYSTALEYE_MACRO=$CRYSTALEYE_SYS/macro/
#
# Set path
export PATH=$CRYSTALEYE_SYS/bin:$PATH
export LD_LIBRARY_PATH=$CRYSTALEYE_SYS/lib:$LD_LIBRARY_PATH
#
################################################################################
echo ""
echo " CrystalEye-settings "
echo ""
################################################################################

