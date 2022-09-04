# HypersonicFOAM

#### Hypersonic gas dynamics code for OpenFOAM

HypersonicFOAM is an extension of [hyStrath](https://github.com/vincentcasseau/hyStrath) written in C++ fashion for the OpenFOAM toolbox.

---
## Includes
In **hyStrath** (look at the [code original version](https://github.com/vincentcasseau/hyStrath) for details):

+ *hyFoam*: a CFD solver for supersonic combusting flows;  
+ *hy2Foam*: a CFD solver for hypersonic reacting flows;  
+ *hy2MhdFoam*: the *hy2Foam* solver with additional MagnetoHydroDynamics (MHD) capabilities.  

Compared to the original version, the code has been modified by improving or implementing from scratch the following new features:

+ Kurganov scheme calculation capability for the convective term in species conservation equations [3];  
+ Electronic energy reactive source term generated during the ionization process [4];  
+ Gupta mixing rule [4];  
+ Attractive (ion–electron) or repulsive (ion–ion and electron–electron) shielded Coulomb potential by Mason *et al.* [5];  
+ Appleton-Bray model for E-T energy transfer [4];  
+ V-T relaxation time models and dissociation rates constants proposed by Shatalov *et al.* [6].  

In **hyPoliMi**:

+ *rhoCentralReactingFoam*: a CFD solver for supersonic reacting flows based on the latest version of *rhoCentralFoam*.

---  
## Compatibility, Download and Installation

### Compatibility
+ hyStrath: [OpenFOAM-v1706](https://sourceforge.net/projects/openfoam/files/v1706)
+ hyPoliMi: [OpenFOAM-v1912](https://sourceforge.net/projects/openfoam/files/v1912)

### Download
```sh
git clone https://github.com/ivanZanardi/hypersonicfoam
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

## Cite HypersonicFOAM

```
@masterthesis{Zanardi2020Thesis,
  author  = {Zanardi, Ivan and Piscaglia, Federico and Frezzotti, Aldo},
  title   = {{Effects of nonequilibrium oxygen dissociation and vibrational relaxation in hypersonic flows}},
  school  = {Politecnico di Milano},
  address = {Via Lambruschini 15, building 20, ground floor, 20158 Milano, Italy},
  year    = {2020},
  url     = {http://hdl.handle.net/10589/154571}
}
@article{Casseau2016Aerospace1,
  author  = {Casseau, Vincent and Palharini, Rodrigo C. and Scanlon, Thomas J. and Brown, Richard E.},
  title   = {{A Two-Temperature Open-Source CFD Model for Hypersonic Reacting Flows, Part One: Zero-Dimensional Analysis}},
  journal = {Aerospace},
  volume  = {3},
  year    = {2016},
  number  = {4},
  url     = {https://www.mdpi.com/2226-4310/3/4/34},
  issn    = {2226-4310}
}
@article{Casseau2016Aerospace2,
  author  = {Casseau, Vincent and Espinoza, Daniel E. R. and Scanlon, Thomas J. and Brown, Richard E.},
  title   = {{A Two-Temperature Open-Source CFD Model for Hypersonic Reacting Flows, Part Two: Multi-Dimensional Analysis}},
  journal = {Aerospace},
  volume  = {3},
  year    = {2016},
  number  = {4},
  url     = {https://www.mdpi.com/2226-4310/3/4/45},
  issn    = {2226-4310}
}
```

---  

## References

[1]  V. Casseau, D. E.R. Espinoza, T. J. Scanlon, and R. E. Brown, [*A Two-Temperature Open-Source CFD Model for Hypersonic Reacting Flows, Part Two: Multi-Dimensional Analysis*](https://www.mdpi.com/2226-4310/3/4/45), Aerospace, vol. 3, no. 4, p. 45, 2016.

[2]  V. Casseau, R. C. Palharini, T. J. Scanlon, and R. E. Brown, [*A Two-Temperature Open-Source CFD Model for Hypersonic Reacting Flows, Part Two: Multi-Dimensional Analysis*](https://www.mdpi.com/2226-4310/3/4/34), Aerospace, vol. 3, no. 4, p. 34, 2016.

[3]  C. J. Greenshields, H. G. Weller, L. Gasparini, and J. M. Reese, [*Implementation of semi-discrete, non-staggered central schemes in a colocated, polyhedral, finite volume framework, for high-speed viscous flows*](https://onlinelibrary.wiley.com/doi/abs/10.1002/fld.2069), International Journal for Numerical Methods in Fluids, 63(1):1–21, 2010.

[4]  P. A. Gnoffo, R. N. Gupta, and J. L. Shinn, [*Conservation equations and physical models for hypersonic air flows in thermal and chemical nonequilibrium*](https://ntrs.nasa.gov/search.jsp?R=19890006744), Technical report, NASA, 1989.

[5]  E. A. Mason, R. J. Munn, and F. J. Smith, [*Transport Coefficients of Ionized Gases*](https://aip.scitation.org/doi/abs/10.1063/1.1762365), The Physics of Fluids, 10(8):1827–1832, 1967.

[6]  L. B. Ibraguimova, A. L. Sergievskaya, V. Yu. Levashov, O. P. Shatalov, Yu. V. Tunik, and I. E. Zabelinskii, [*Investigation of oxygen dissociation and vibrational relaxation at temperatures 4000-10800 K*](https://doi.org/10.1063/1.4813070), The Journal of Chemical Physics, 139(3):034317, 2013.
