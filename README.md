# cern_get

Download cern opendata to csv using CernVM, root and fwlite.

## How to use

* download and start CernVM via virtualbox (http://opendata.cern.ch/record/250)
* run terminal and execute following commands:

```bash
cmsrel CMSSW_4_2_8
cd CMSSW_4_2_8/src/
cmsenv
cd
```

* Then download this repo

```bash
git clone git@github.com:ffloyd/cern_get.git
cd cern_get/
```

* Download index file from http://opendata.cern.ch/record/8 As example:

```bash
wget -O index0000.txt http://opendata.cern.ch/record/8/files/CMS_Run2010B_MinimumBias_AOD_Apr21ReReco-v1_0000_file_index.txt
```

* Index file may contain over 200 entries. It'll take very long time to process. It'll be good idea to reduce index for first time.
* Then start root

```bash
root -l
```

* Following commands should be executed in root console
* Load fwlite env

```
.x init_env.C
```

* Load and compile downloader. It'll produce a lot of warning messages - it's ok.

```
.L ak5CaloJets_to_csv.C++
```

* Then load you index

```cpp
vector<string> files = loadFileIndex("index0000.txt");
```

* Download reco::CaloJets information to csv

```cpp
downloadCaloJets(files, "events.csv", "jets.csv");
```

* wait to complete and you'll get two csv files
