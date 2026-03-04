# Changelog: WBMsed

All notable changes to this project will be documented in this file.

---

## [4.4.1] — 2021-10-25

* Implemented new $D_s$, $W$, and depth equations derived from Tom Ashley's data for both bedload and SBM modules. 



## [4.4.0] — 2021-10-21

* Added Contributing Area as an input layer rather than calculating it dynamically.

## [4.3.9] — 2021-10-13

* Updated particle size ($D_s$) calculation in the bedload module to be a function of suspended sediment alone.
* Removed Ashley's bedload components.

## [4.3.8] — 2021-10-01

* Enabled calculation of daily velocity within the SBM module.

## [4.3.7] — 2021-09-07

* Added wash load calculation (Qs-SBM).
* Adjusted water density parameters relative to wash load.

## [4.3.6] — 2021-07-01

* Mean discharge is now calculated via the `DischargeMean` module rather than within the sediment module.

## [4.3.5] — 2021-06

* Moved Mean Discharge calculation to `SedimentFlux` (formerly in `MeanDischarge`). 


* Modified the bankfull component within `Discharge_w_bankfull`. 



## [4.3.4] — 2021-06

* Updated $\Psi$ calculation using an empirical adjustment to $\bar{C}$.
* Streamlined temperature calculations using `AvgNSteps`, removing the requirement for Mean Input layers.

## [4.3.3] — 2021-06

* Fixed $T_e$ by weighting it by Area ($A$) instead of Discharge ($Q$).

## [4.3.0] — 2021-06

* Created a dedicated suspended bed module (Syvitski).
* Added the `TotalLoad` parameter.

## [4.2.0] — 2020-08

* Added sediment trapping input options using the Grill et al. (2019) dataset.

## [4.1.3] — 2020-04

* Updated Ashley bedload and $D_s$ equations within `MDBedloadFlux.c`.

## [4.1.2] — 2020-02

* Integrated new bedload equations in `MDBedloadFlux.c` (Ashley, and Lammers et al. 2018).
* Updated `Discharge_w_bankful` to allow for "no bankfull $Q$" by setting a high threshold ($1,000,000$).

## [4.1.1] — 2019-10

* Added the Syvitski et al. (2019) bedload equation to `MDBedloadFlux.c`. 



## [4.1.0] — 2019-04/05

* Updated `MDBedloadFlux.c` with new coefficient values for the modified Bagnold equation. 


* Added three additional bedload equations. 



## [4.0.0] — 2019-02/03

### March 2019 updates

* Updated `MDRiverbedShapeExp.c` to use `MDDischargeBFDef` instead of `MeanDischarge` for velocity calculations.
* Added a velocity threshold parameter to the bedload module.

### February 2019 updates

* Updated the Mean Discharge module (`MDDicshargeMean.c`) to utilize the $Q_{bf}$ module (`MDDischargeBFDef`). 


* Updated `MDRiverbedShapeExp.c` to use `MDDischargeBFDef` for velocity calculations. 


* Modified riverbed width and depth coefficients. 



## [1.1.0] — 2013-06-20

* Calibrated the $\Psi$ equation: the first term of the $c$ coefficient was adjusted from $1.4$ to $0.25$. 
