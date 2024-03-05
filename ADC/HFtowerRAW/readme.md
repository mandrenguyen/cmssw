## Get HF tower info from RAW data

```
# In src/
git clone git@github.com:boundino/HltL1Run2021.git
ln -s HltL1Run2021/L1/ADC .
scram b -j4
cp HltL1Run2021/L1/ADC/HFtowerRAW/python/hfsim_minimal_cfg.py .
# run with grid cert set 
cmsRun hfsim_minimal_cfg.py
```

In the output, `hft` is the HF energy deposit (corresponding to `HiHF` in forest).

```
   (hfoutput.root)
   ./
    └── (TDirectoryFile) => hft
        └── (TTree) => HFtree (146)
******************************************************************************
*Tree    :HFtree    : hfs                                                    *
*Entries :      146 : Total =           10623 bytes  File  Size =       4141 *
*        :          : Tree compression factor =   1.77                       *
******************************************************************************
*Br    0 :run       : run/I                                                  *
*Entries :      146 : Total  Size=       1133 bytes  File Size  =        101 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   6.50     *
*............................................................................*
*Br    1 :lumi      : lumi/I                                                 *
*Entries :      146 : Total  Size=       1138 bytes  File Size  =        107 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   6.14     *
*............................................................................*
*Br    2 :event     : event/I                                                *
*Entries :      146 : Total  Size=       1143 bytes  File Size  =        393 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.67     *
*............................................................................*
*Br    3 :bx        : bx/I                                                   *
*Entries :      146 : Total  Size=       1128 bytes  File Size  =         97 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   6.75     *
*............................................................................*
*Br    4 :nhf4p     : nhf4p/I                                                *
*Entries :      146 : Total  Size=       1143 bytes  File Size  =        345 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.91     *
*............................................................................*
*Br    5 :nhf4n     : nhf4n/I                                                *
*Entries :      146 : Total  Size=       1143 bytes  File Size  =        350 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.88     *
*............................................................................*
*Br    6 :hft       : hft/F                                                  *
*Entries :      146 : Total  Size=       1133 bytes  File Size  =        645 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.02     *
*............................................................................*
*Br    7 :hftp      : hftp/F                                                 *
*Entries :      146 : Total  Size=       1138 bytes  File Size  =        653 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.01     *
*............................................................................*
*Br    8 :hftm      : hftm/F                                                 *
*Entries :      146 : Total  Size=       1138 bytes  File Size  =        653 *
*Baskets :        1 : Basket Size=      32000 bytes  Compression=   1.01     *
*............................................................................*
```

