# HypersonicFOAM

## Overview

HypersonicFOAM is an extended suite of solvers for hypersonic gas dynamics, developed within the OpenFOAM framework. It builds upon the foundations of [hyStrath](https://github.com/vincentcasseau/hyStrath), adopting a modular and object-oriented C++ approach to enable advanced modeling of reacting, ionizing, and magnetized flows.

---

## Features and Capabilities

### **hyStrath**

Refer to the [hyStrath](https://github.com/vincentcasseau/hyStrath) repository for additional context and development history. The original codebase provides:

- `hyFoam`: Solver for supersonic combusting flows.
- `hy2Foam`: Solver for hypersonic reacting flows.
- `hy2MhdFoam`: Extension of `hy2Foam` with MagnetoHydroDynamics (MHD) functionality.

This fork introduces several enhancements and new physics models, including:

- Kurganov scheme implementation for convective terms in species transport equations [3].
- Inclusion of electronic energy source terms due to ionization [4].
- Gupta mixing rules for thermochemical properties [4].
- Shielded Coulomb interactions (ion–electron, ion–ion, electron–electron) using Mason *et al.*'s approach [5].
- Appleton–Bray model for electron–translational (E–T) energy exchange [4].
- Shatalov's models for vibrational–translational (V–T) relaxation and oxygen dissociation [6].

### **hyPoliMi**

This extension includes:

- `rhoCentralReactingFoam`: A supersonic reacting flow solver based on OpenFOAM's `rhoCentralFoam`, adapted for high-temperature gas dynamics.

---

## Installation

### Compatibility

| Solver Suite     | Compatible OpenFOAM Version |
|------------------|-----------------------------|
| hyStrath         | [v1706](https://sourceforge.net/projects/openfoam/files/v1706)         |
| hyPoliMi         | [v1912](https://sourceforge.net/projects/openfoam/files/v1912)         |

### Download

```bash
git clone https://github.com/ivanZanardi/hypersonicfoam
```

### Build Instructions

#### For **hyStrath**:

```bash
cd hyStrath/  
./install-all.sh <np> 2>&1 | tee log.install
```

#### For **hyPoliMi**:

```bash
cd hyPoliMi/  
./install.sh <np> 2>&1 | tee log.install
```

Replace `<np>` with the desired number of processors for parallel compilation.

---

## Citation

If you use **HypersonicFOAM** in your research, please cite:

```bibtex
@masterthesis{Zanardi2020Thesis,
  author  = {Ivan Zanardi},
  title   = {Effects of nonequilibrium oxygen dissociation and vibrational relaxation in hypersonic flows},
  school  = {Politecnico di Milano},
  address = {Via Lambruschini 15, building 20, ground floor, 20158 Milano, Italy},
  year    = {2020},
  url     = {http://hdl.handle.net/10589/154571}
}
```

This project is built upon hyStrath [1,2].

---

## References

1. Casseau, V., Espinoza, D. E. R., Scanlon, T. J., & Brown, R. E. (2016). *A two-temperature open-source CFD model for hypersonic reacting flows, Part Two: Multi-dimensional analysis*. Aerospace, **3**(4), 45. https://doi.org/10.3390/aerospace3040045

2. Casseau, V., Palharini, R. C., Scanlon, T. J., & Brown, R. E. (2016). *A two-temperature open-source CFD model for hypersonic reacting flows, Part One: Zero-dimensional analysis*. Aerospace, **3**(4), 34. https://doi.org/10.3390/aerospace3040034

3. Greenshields, C. J., Weller, H. G., Gasparini, L., & Reese, J. M. (2010). *Implementation of semi-discrete, non-staggered central schemes in a colocated, polyhedral, finite volume framework, for high-speed viscous flows*. International Journal for Numerical Methods in Fluids, **63**(1), 1–21. https://doi.org/10.1002/fld.2069

4. Gnoffo, P. A., Gupta, R. N., & Shinn, J. L. (1989). *Conservation equations and physical models for hypersonic air flows in thermal and chemical nonequilibrium* (NASA Technical Memorandum 101440). NASA Langley Research Center. https://ntrs.nasa.gov/citations/19890006744

5. Mason, E. A., Munn, R. J., & Smith, F. J. (1967). *Transport coefficients of ionized gases*. Physics of Fluids, **10**(8), 1827–1832. https://doi.org/10.1063/1.1762365

6. Ibraguimova, L. B., Sergievskaya, A. L., Levashov, V. Y., Shatalov, O. P., Tunik, Y. V., & Zabelinskii, I. E. (2013). *Investigation of oxygen dissociation and vibrational relaxation at temperatures 4000–10800 K*. The Journal of Chemical Physics, **139**(3), 034317. https://doi.org/10.1063/1.4813070
