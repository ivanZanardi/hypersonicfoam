# HypersonicFoam

#### Hypersonic gas dynamics code for OpenFOAM toolbox.

---
## Includes
In **hyStrath** (look at the [code original version](https://github.com/vincentcasseau/hyStrath) for details):

+ *hyFoam*: a CFD solver for supersonic combusting flows;  
+ *hy2Foam*: a CFD solver for hypersonic reacting flows;  
+ *hy2MhdFoam*: the *hy2Foam* solver with additional MagnetoHydroDynamics (MHD) capabilities.  

Compared to the original version, the code has been modified by improving or implementing from scratch the following new features:

+ Kurganov scheme calculation capability for the convective term in species conservation equations [1];  
+ Electronic energy reactive source term generated during the ionization process [2];  
+ Gupta mixing rule [2];  
+ Attractive (ion–electron) or repulsive (ion–ion and electron–electron) shielded Coulomb potential by Mason *et al.* [3];  
+ Appleton-Bray model for E-T energy transfer [2];  
+ V-T relaxation time models and dissociation rates constants proposed by Shatalov *et al.* [4].  

In **hyPoliMi**:

+ *rhoCentralReactingFoam*: a CFD solver for supersonic reacting flows based on the latest version of *rhoCentralFoam*.

---  
## Compatibility, Download and Installation

### Compatibility
+ hyStrath: [OpenFOAM-v1706](https://sourceforge.net/projects/openfoam/files/v1706)
+ hyPoliMi: [OpenFOAM-v1912](https://sourceforge.net/projects/openfoam/files/v1912)

### Download
```sh
git clone https://github.com/ivanZanardi/hypersonicFoam
```

### Installation
For **hyStrath**:
```sh
cd hyStrath/  
./install-all.sh 2 2>&1 | tee log.install
```

For **hyPoliMi**:
```sh
cd hyPoliMi/  
./install.sh 2 2>&1 | tee log.install
```

where _2_ is the number of processors to be used during the installation.

---  

## Publications

I. Zanardi. *Effects of nonequilibrium oxygen dissociation and vibrational relaxation in hypersonic flows*. Master thesis, Politecnico di Milano, Italy, April 2020.

```sh
@article{Zanardi2020Thesis,
  author  = {Lu, Lu and Meng, Xuhui and Mao, Zhiping and Karniadakis, George Em},
  title   = {{Effects of nonequilibrium oxygen dissociation and vibrational relaxation in hypersonic flows}},
  journal = {SIAM Review},
  volume  = {63},
  number  = {1},
  pages   = {208-228},
  year    = {2021},
  doi     = {10.1137/19M1274067}
}
```

---  

## Citations

[1]  C. J. Greenshields, H. G. Weller, L. Gasparini, and J. M. Reese. [*Implementation of semi-discrete, non-staggered central schemes in a colocated, polyhedral, finite volume framework, for high-speed viscous flows*](https://onlinelibrary.wiley.com/doi/abs/10.1002/fld.2069). International Journal for Numerical Methods in Fluids, 63(1):1–21, 2010.

[2]  P. A. Gnoffo, R. N. Gupta, and J. L. Shinn. [*Conservation equations and physical models for hypersonic air flows in thermal and chemical nonequilibrium*](https://ntrs.nasa.gov/search.jsp?R=19890006744). Technical report, NASA, 1989.

[3]  E. A. Mason, R. J. Munn, and F. J. Smith. [*Transport Coefficients of Ionized Gases*](https://aip.scitation.org/doi/abs/10.1063/1.1762365). The Physics of Fluids, 10(8):1827–1832, 1967.

[4]  L. B. Ibraguimova, A. L. Sergievskaya, V. Yu. Levashov, O. P. Shatalov, Yu. V. Tunik, and I. E. Zabelinskii. [*Investigation of oxygen dissociation and vibrational relaxation at temperatures 4000-10800 K*](https://doi.org/10.1063/1.4813070). The Journal of Chemical Physics, 139(3):034317, 2013.
