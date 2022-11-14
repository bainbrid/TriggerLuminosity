# TriggerLuminosity

```
cmsrel CMSSW_12_4_8
cd CMSSW_12_4_8/src
cmsenv
git cms-init
git remote add bainbrid git@github.com:bainbrid/cmssw.git
git cms-merge-topic -u bainbrid:fix_LumiList_124X
git clone git@github.com:bainbrid/TriggerLuminosity.git PhysicsTools/TriggerLuminosity
scram b
cd PhysicsTools/TriggerLuminosity/test
. run.sh
```
