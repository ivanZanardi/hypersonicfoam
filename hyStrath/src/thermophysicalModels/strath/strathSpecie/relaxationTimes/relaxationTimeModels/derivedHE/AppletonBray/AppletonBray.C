/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright held by original author
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

\*---------------------------------------------------------------------------*/

#include "AppletonBray.H"
#include "fvm.H"

// * * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * //

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class ThermoType>
Foam::AppletonBray<ThermoType>::AppletonBray
(
    rho2ReactionThermo& thermo,
    const compressibleTurbulenceModel& turbulence
)
:
    relaxationTimeModelHE(thermo, turbulence),
    
    speciesThermo_
    (
        dynamic_cast<const multi2ComponentMixture<ThermoType>&>
            (this->thermo_).speciesData()
    ),
    
    electronListPosition_(species()["e-"]),

    sigmaCoeff_(species().size()), // Cross section by Gnoffo et al. 1989

    RR(constant::physicoChemical::R.value()),
    NA(constant::physicoChemical::NA.value()),
    kB(RR/NA),
    ec(Foam::constant::electromagnetic::e.value()),
    pi(constant::mathematical::pi)
{   
    FixedList<scalar, 3> initList(0.0);		// NEW ZANARDI
    initList[0] = readScalar(
	this->subDict("AppletonBrayCoefficients").lookup("constantFactor"));

    forAll(sigmaCoeff_, speciei)
    {
	if(this->crossSection_ == "constant")
	{
	    sigmaCoeff_.set
	    (
		speciei, new FixedList<scalar, 3>(initList)
	    );
	}
	else if(this->crossSection_ == "lineFitted")
	{
	    sigmaCoeff_.set
	    (
		speciei, 
		new FixedList<scalar, 3>
		(
		     this->subDict("AppletonBrayCoefficients")
		    .subDict("lineFittedCoeffs")
		    .lookupOrDefault(species()[speciei], initList)
		)
	    );
	}
    }
}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

template<class ThermoType>
void Foam::AppletonBray<ThermoType>::correct()
{
    // Scalabrin PhD thesis, Eq.(2.65)
    const volScalarField& Tt = thermo_.Tt();
    const volScalarField& pDe = thermo_.composition().pD(electronListPosition_);
    const volScalarField& nDe = thermo_.composition().nD(electronListPosition_);
    const volScalarField& Te = thermo_.composition().Tv(electronListPosition_);
    
    const scalarField& TtCells = Tt.internalField();
    const scalarField& pDeCells = pDe.internalField();
    const scalarField& nDeCells = nDe.internalField();
    const scalarField& TeCells = Te.internalField();
    scalarField& QHECells = this->QHE_.primitiveFieldRef();
    
    QHECells = 0.0;
    
    forAll(Tt.boundaryField(), patchi)
    {
        fvPatchScalarField& pQHE = this->QHE_.boundaryFieldRef()[patchi];
        pQHE = 0.0;
    }

    forAll(species(), specier)
    {
        if(specier != electronListPosition_) 
        { 
            const volScalarField& pDr = thermo_.composition().pD(specier);
            const scalarField& pDrCells = pDr.internalField();
            
            if(speciesThermo_[specier].particleType() < 3)
            {
                forAll(pDrCells, celli)
                {        
                    scalar sigma_er = sigmaCoeff_[specier][0]+sigmaCoeff_[specier][1]*TeCells[celli]
					+sigmaCoeff_[specier][2]*pow(TeCells[celli],2);
                    QHECells[celli] += sigma_er*pDr[celli]/sqr(W(specier));
                }
                
                forAll(pDr.boundaryField(), patchi)
                {
                    const fvPatchScalarField& ppDr = pDr.boundaryField()[patchi];
                    const fvPatchScalarField& pTe = Te.boundaryField()[patchi];
                    fvPatchScalarField& pQHE = this->QHE_.boundaryFieldRef()[patchi];
                    
                    forAll(ppDr, facei)
                    {
			scalar sigma_er = sigmaCoeff_[specier][0]+sigmaCoeff_[specier][1]*pTe[facei]
					    +sigmaCoeff_[specier][2]*pow(pTe[facei],2);
                        pQHE[facei] += sigma_er*ppDr[facei]/sqr(W(specier));
                    }
                }
            }
            else
            {
		forAll(pDrCells, celli)
		{
		    if(nDeCells[celli] > Foam::ROOTVSMALL)
		    {
			scalar sigma_eIon = 8.0*pi*pow4(ec)/(27.0*sqr(kB*TeCells[celli]))*
				    log(1.0+(9.0*pow3(kB*TeCells[celli]))/(4.0*pi*nDeCells[celli]*pow6(ec)));

			QHECells[celli] += sigma_eIon*pDr[celli]/sqr(W(specier));
		    }
		}                

                forAll(pDr.boundaryField(), patchi)
                {
                    const fvPatchScalarField& pnDe = nDe.boundaryField()[patchi];
                    const fvPatchScalarField& pTe = Te.boundaryField()[patchi];
                    const fvPatchScalarField& ppDr = pDr.boundaryField()[patchi];
                    fvPatchScalarField& pQHE = this->QHE_.boundaryFieldRef()[patchi];
                    
		    forAll(ppDr, facei)
		    {		    
			if(pnDe[facei] > Foam::ROOTVSMALL)
			{
			    scalar sigma_eIon = 8.0*pi*pow4(ec)/(27.0*sqr(kB*pTe[facei]))*
					log(1.0+(9.0*pow3(kB*pTe[facei]))/(4.0*pi*pnDe[facei]*pow6(ec)));

			    pQHE[facei] += sigma_eIon*ppDr[facei]/sqr(W(specier));
			}
		    }        
                }
            }
        } 
    }//end species loop

    forAll(TtCells, celli)
    {        
        QHECells[celli] *= 3.0*RR*pDeCells[celli]*(TtCells[celli]-TeCells[celli])*NA
            *sqrt(8.0*RR*TeCells[celli]/(pi*W(electronListPosition_)));
    }
    
    forAll(Tt.boundaryField(), patchi)
    {
        const fvPatchScalarField& pTt = Tt.boundaryField()[patchi];
        const fvPatchScalarField& pTe = Te.boundaryField()[patchi];
        const fvPatchScalarField& ppDe = pDe.boundaryField()[patchi];
        fvPatchScalarField& pQHE = this->QHE_.boundaryFieldRef()[patchi];
        
        forAll(pTt, facei)
        {        
            pQHE[facei] *= 3.0*RR*ppDe[facei]*(pTt[facei]-pTe[facei])*NA
                *sqrt(8.0*RR*pTe[facei]/(pi*W(electronListPosition_)));
        }
    }
}  
    

template<class ThermoType>
bool Foam::AppletonBray<ThermoType>::read()
{
    if (regIOobject::read())
    {
        return true;
    }
    else
    {
        return false;
    }
}
   

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
