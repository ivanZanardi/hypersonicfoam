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

#include "GuptaMR.H"
#include "fvm.H"

#include<sstream>

/* * * * * * * * * * * * * * * public constants  * * * * * * * * * * * * * * */

// Avogadro's number
template<class ThermoType>
const Foam::scalar Foam::GuptaMR<ThermoType>::NA = Foam::constant::physicoChemical::NA.value();

// Boltzmann's constant
template<class ThermoType>
const Foam::scalar Foam::GuptaMR<ThermoType>::kB = Foam::constant::physicoChemical::k.value();

//- Universal gas constant (in [J/(mol K)])
template<class ThermoType>
const Foam::scalar Foam::GuptaMR<ThermoType>::Ru = Foam::constant::physicoChemical::R.value();

//- Mathematical constant Pi
template<class ThermoType>
const Foam::scalar Foam::GuptaMR<ThermoType>::pi = Foam::constant::mathematical::pi;

//- Fundamental electric charge in CGS units
template<class ThermoType>
const Foam::scalar Foam::GuptaMR<ThermoType>::eCGS = 4.8032e-10;


// * * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * //

template <class ThermoType>
Foam::string Foam::GuptaMR<ThermoType>::numberToString(int number)
{
   std::ostringstream ss;
   ss << number;
   return ss.str();
}
  

template<class ThermoType>
void Foam::GuptaMR<ThermoType>::piOmegaNeutralInit(label no, label i, label j)
{
    word year = word::null;
    word data = word::null;
    word typeOmega = numberToString(no+1)+numberToString(no+1);
    word omegaData = dict_.subDict("transportModels").lookup("binaryDiffusivityModel");

    if(omegaData == "GuptaO" or omegaData == "GuptaD")
    {
        year = word(dict_.subDict("transportModels")
            .subDict("diffusiveFluxesParameters").lookup("yearGuptaModel"));
	data = "Gupta";
    }
    else
    {
        year = "2005";
	data = "Wright";
    }

    FixedList<scalar,4> defaultList;
    forAll(defaultList, i)
    {
        defaultList[i] = 0.0;
    }

    if (dict_.subDict("collisionData").subDict("involvingNeutral").subDict(data+year+"O")
	    .subDict("Omega"+typeOmega).found(species()[i]+"_"+species()[j]))
    {
	piOmegaNeutral_[no][i][j] = dict_.subDict("collisionData").subDict("involvingNeutral").subDict(data+year+"O")
	    .subDict("Omega"+typeOmega).lookupOrDefault<FixedList<scalar,4>>(species()[i]+"_"+species()[j], defaultList);
    }
    else if (dict_.subDict("collisionData").subDict("involvingNeutral").subDict(data+year+"O")
	    .subDict("Omega"+typeOmega).found(species()[j]+"_"+species()[i]))
    {
	piOmegaNeutral_[no][i][j] = dict_.subDict("collisionData").subDict("involvingNeutral").subDict(data+year+"O")
	    .subDict("Omega"+typeOmega).lookupOrDefault<FixedList<scalar,4>>(species()[j]+"_"+species()[i], defaultList);
    }
    else
    {
	FatalErrorIn("void Foam::GuptaMR<ThermoType>::piOmegaNeutralInit(label no, label i, label j)")
	    << "Collision integral data missing for species couple (" << species()[i] << ", " << species()[j] << ")."
	    << exit(FatalError);
    }
}


template<class ThermoType>
void Foam::GuptaMR<ThermoType>::piOmegaNonNeutralsInit(const word& attractionType, label no)
{
    word typeOmega = numberToString(no+1)+numberToString(no+1);
    
    label potentialType = 0;
    if (attractionType == "attractive")
    {
        potentialType = 1;
    }
    
    if (dict_.subDict("collisionData").subDict("shieldedCoulombPotential")
            .subDict("Omega"+typeOmega).isDict(attractionType))
    {
        piOmegaNonNeutral_[potentialType][no][0] = readScalar(dict_.subDict("collisionData")
            .subDict("shieldedCoulombPotential").subDict("Omega"+typeOmega).subDict(attractionType).lookup("cn"));
        piOmegaNonNeutral_[potentialType][no][1] = readScalar(dict_.subDict("collisionData")
            .subDict("shieldedCoulombPotential").subDict("Omega"+typeOmega).subDict(attractionType).lookup("Cn"));
        piOmegaNonNeutral_[potentialType][no][2] = readScalar(dict_.subDict("collisionData")
            .subDict("shieldedCoulombPotential").subDict("Omega"+typeOmega).subDict(attractionType).lookup("Dn"));
    }
    else
    {
        FatalErrorIn("void Foam::GuptaMR<ThermoType>::piOmegaNonNeutralsInit(word attractionType, label no)")
            << "Dictionary or dictionary entry not found"
            << exit(FatalError);
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class ThermoType>
Foam::GuptaMR<ThermoType>::GuptaMR
(
    rho2ReactionThermo& thermo,
    const compressibleTurbulenceModel& turbulence
)
:
    mixingRule(thermo, turbulence),

    speciesThermo_
    (
        dynamic_cast<const multi2ComponentMixture<ThermoType>&>
            (this->thermo_).speciesData()
    ),
    
    dict_
    (
        thermo.transportDictionary()
    )
{
    noNeutralParticles_ = 0;
    forAll(species(), speciei)
    {
        if(speciesThermo_[speciei].particleCharge() == 0)
        {
            noNeutralParticles_ += 1;
        }
    }
    
    if(species().size() - noNeutralParticles_ != 0)
    {
        for(label k=0; k <= 1; k++)
        {
            piOmegaNonNeutralsInit("attractive", k);
            piOmegaNonNeutralsInit("repulsive", k);
        }
    }
    
    forAll(piOmegaNeutral_, k)
    {
        piOmegaNeutral_[k].setSize(species().size()); // It will be filled only
						      // with neutral particles data
        forAll(piOmegaNeutral_[k], speciei)
        { 
            piOmegaNeutral_[k].set
            (
                speciei, 
                new PtrList<FixedList<scalar, 4> >(species().size())
            );
        }
    }
    
    forAll(piOmegaNeutral_, k)
    {
        forAll(piOmegaNeutral_[k], speciei)
        { 
            forAll(piOmegaNeutral_[k][speciei], speciej)
            {
                piOmegaNeutral_[k][speciei].set
                (
                    speciej, 
                    new FixedList<scalar, 4>
                );
            }
        }
    }

    for(label k=0; k <= 1; k++)
    {
	forAll(species(), speciei)
	{
	    if(speciesThermo_[speciei].particleCharge() == 0)
	    {
		forAll(species(), speciej)
		{
		    piOmegaNeutralInit(k, speciei, speciej);
		}
	    }
	}
    }
    
    correct();
}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

template<class ThermoType>
void Foam::GuptaMR<ThermoType>::correct()
{
    const volScalarField& Tt = thermo_.Tt();
    const volScalarField& p = thermo_.p();
    const volScalarField& rho = thermo_.rho();
    
    const scalarField& TtCells = Tt.internalField();
    const scalarField& pCells = p.internalField();
    const scalarField& rhoCells = rho.internalField();
    
    volScalarField& muMix = thermo_.mu();
    volScalarField& kappatrMix = thermo_.kappatr();
    volScalarField& kappaveMix = thermo_.kappave();
    volScalarField& alphatrMix = thermo_.alphatr();
    volScalarField& alphaveMix = thermo_.alphave();
    
    volScalarField tmpMu = muMix*0.0;
    volScalarField tmpKappat = kappatrMix*0.0;
    volScalarField tmpKappar = kappatrMix*0.0;
    volScalarField tmpKappave = kappaveMix*0.0;
    volScalarField tmpAlphave = alphaveMix*0.0;
    
    scalarField& muCells = tmpMu.primitiveFieldRef();
    scalarField& kappatCells = tmpKappat.primitiveFieldRef();
    scalarField& kapparCells = tmpKappar.primitiveFieldRef();
    scalarField& kappaveCells = tmpKappave.primitiveFieldRef();
    scalarField& alphaveCells = tmpAlphave.primitiveFieldRef();

    // Update species data
    forAll(species(), speciei)
    {
        const volScalarField& Tve = thermo_.composition().Tv(speciei);
        const scalarField& TveCells = Tve.internalField();
        
        scalarField& spmuCells = spmu_[speciei].primitiveFieldRef();
        scalarField& spkappatrCells = spkappatr_[speciei].primitiveFieldRef();
        scalarField& spkappaveCells = spkappave_[speciei].primitiveFieldRef();
        scalarField& spalphatrCells = spalphatr_[speciei].primitiveFieldRef();
        scalarField& spalphaveCells = spalphave_[speciei].primitiveFieldRef();

        if(speciesThermo_[speciei].particleCharge() == 0) // The particle has a neutral charge
        {
            forAll(TtCells, celli)
            {
                spmuCells[celli] = ms(speciei)/collisionIntegralNeutral2(speciei, speciei, TtCells[celli]);

                spkappatrCells[celli] = kB*(15.0/4.0/collisionIntegralNeutral2(speciei, speciei, TtCells[celli])
                    + 1.0/collisionIntegralNeutral1(speciei, speciei, TtCells[celli]));
                spkappaveCells[celli] = kB*Cv_vel(speciei, pCells[celli], TveCells[celli])
                    *W(speciei)/Ru/collisionIntegralNeutral1(speciei, speciei, TtCells[celli]);
                spalphatrCells[celli] = spkappatrCells[celli]/Cv_t(speciei, pCells[celli], TtCells[celli]);
                if(Cv_vel(speciei, pCells[celli], TveCells[celli]) != 0.0)
                {
                    spalphaveCells[celli] = spkappaveCells[celli]/Cv_vel(speciei, pCells[celli], TveCells[celli]);
                }
            }
        }    
        else // The particle is an ion or an electron
        {
            const volScalarField& nDe = thermo_.composition().nD("e-");
            const scalarField& nDeCells = nDe.internalField();

            if(speciesThermo_[speciei].particleCharge() == 1) // The particle is an ion
            {
                forAll(TtCells, celli)
                {
                    spmuCells[celli] = ms(speciei)/collisionIntegralNonNeutral2(speciei, speciei, TveCells[celli], nDeCells[celli]);
                    spkappatrCells[celli] = kB*(15.0/4.0/collisionIntegralNonNeutral2(speciei, speciei, TveCells[celli], nDeCells[celli]) 
                        + 1.0/collisionIntegralNonNeutral1(speciei, speciei, TveCells[celli], nDeCells[celli]));
                    spkappaveCells[celli] = kB*Cv_vel(speciei, pCells[celli], TveCells[celli])
                        *W(speciei)/Ru/collisionIntegralNonNeutral1(speciei, speciei, TtCells[celli], nDeCells[celli]);
                    spalphatrCells[celli] = spkappatrCells[celli]/Cv_t(speciei, pCells[celli], TtCells[celli]);
                    if(Cv_vel(speciei, pCells[celli], TveCells[celli]) != 0.0)
                    {
                        spalphaveCells[celli] = spkappaveCells[celli]/Cv_vel(speciei, pCells[celli], TveCells[celli]);
                    }
                }
            }
            else // The particle is an electron
            {
                forAll(TtCells, celli)
                {
                    spmuCells[celli] = ms(speciei)/collisionIntegralNonNeutral2(speciei, speciei, TveCells[celli], nDeCells[celli]);
                    spkappatrCells[celli] = 0.0;
                    spkappaveCells[celli] = 15.0/4.0*kB
                        /(1.45*collisionIntegralNonNeutral2(speciei, speciei, TveCells[celli], nDeCells[celli]));
                    if(Cv_vel(speciei, pCells[celli], TveCells[celli]) != 0.0)
                    {
                        spalphaveCells[celli] = spkappaveCells[celli]/Cv_vel(speciei, pCells[celli], TveCells[celli]);
                    }
                }    
            }
        }

        forAll(Tt.boundaryField(), patchi)
        {        
            const fvPatchScalarField& pTt = Tt.boundaryField()[patchi];
            const fvPatchScalarField& pTve = Tve.boundaryField()[patchi];
            const fvPatchScalarField& pp = p.boundaryField()[patchi];
            const fvPatchScalarField& prho = rho.boundaryField()[patchi];
            
            fvPatchScalarField& pspmu = spmu_[speciei].boundaryFieldRef()[patchi];
            fvPatchScalarField& pspkappatr = spkappatr_[speciei].boundaryFieldRef()[patchi];
            fvPatchScalarField& pspkappave = spkappave_[speciei].boundaryFieldRef()[patchi];
            fvPatchScalarField& pspalphatr = spalphatr_[speciei].boundaryFieldRef()[patchi];
            fvPatchScalarField& pspalphave = spalphave_[speciei].boundaryFieldRef()[patchi];
    
	    if(speciesThermo_[speciei].particleCharge() == 0) // The particle has a neutral charge
            {
                forAll(pTt, facei)
                { 
                    pspmu[facei] = ms(speciei)/collisionIntegralNeutral2(speciei, speciei, pTt[facei]);

                    pspkappatr[facei] = kB*(15.0/4.0/collisionIntegralNeutral2(speciei, speciei, pTt[facei])
                        + 1.0/collisionIntegralNeutral1(speciei, speciei, pTt[facei]));
                    pspkappave[facei] = kB*Cv_vel(speciei, pp[facei], pTt[facei])
                        *W(speciei)/Ru/collisionIntegralNeutral1(speciei, speciei, pTt[facei]);
                    pspalphatr[facei] = pspkappatr[facei]/Cv_t(speciei, pp[facei], pTt[facei]);

                    if(Cv_vel(speciei, pp[facei], pTve[facei]) != 0.0)
                    {
                        pspalphave[facei] = pspkappave[facei]/Cv_vel(speciei, pp[facei], pTve[facei]);
                    }
                }
            }    
	    else // The particle is an ion or an electron
            {
                const fvPatchScalarField& pnDe = thermo_.composition().nD("e-").boundaryField()[patchi];
                
		if(speciesThermo_[speciei].particleCharge() == 1) // The particle is an ion
                {
                    forAll(prho, facei)
                    {
                        pspmu[facei] = ms(speciei)/collisionIntegralNonNeutral2(speciei, speciei, pTt[facei], pnDe[facei]);
                        pspkappatr[facei] = kB*(15.0/4.0/collisionIntegralNonNeutral2(speciei, speciei, pTve[facei], pnDe[facei]) 
                            + 1.0/collisionIntegralNonNeutral1(speciei, speciei, pTve[facei], pnDe[facei]));
                        pspkappave[facei] = kB*Cv_vel(speciei, pp[facei], pTve[facei])
                            *W(speciei)/Ru/collisionIntegralNonNeutral1(speciei, speciei, pTt[facei], pnDe[facei]);
                        pspalphatr[facei] = pspkappatr[facei]/Cv_t(speciei, pp[facei], pTt[facei]);
                        if(Cv_vel(speciei, pp[facei], pTve[facei]) != 0.0)
                        {
                            pspalphave[facei] = pspkappave[facei]/Cv_vel(speciei, pp[facei], pTve[facei]);
                        }
                    }
                }
                else // The particle is an electron
                {
                    forAll(prho, facei)
                    {
                        pspmu[facei] = ms(speciei)/collisionIntegralNonNeutral2(speciei, speciei, pTve[facei], pnDe[facei]);
                        pspkappatr[facei] = 0.0;
                        pspkappave[facei] = 15.0/4.0*kB
                            /(1.45*collisionIntegralNonNeutral2(speciei, speciei, pTve[facei], pnDe[facei]));
                        if(Cv_vel(speciei, pp[facei], pTve[facei]) != 0.0)
                        {
                            pspalphave[facei] = pspkappave[facei]/Cv_vel(speciei, pp[facei], pTve[facei]);
                        }
                    }
                }
            }
        }
    } // End update species data

    
    // Update mixture data
    forAll(species(), speciei)
    {
        if(speciesThermo_[speciei].particleType() > 0) 
        { 
            const volScalarField& pD = thermo_.composition().pD(speciei);
            volScalarField den1 = pD*0.0;
            volScalarField denkt1 = pD*0.0;
            volScalarField denkt2 = pD*0.0;
            volScalarField denkr1 = pD*0.0;
            volScalarField denkr2 = pD*0.0;
            
            const scalarField& pDCells = pD.internalField();
            scalarField& den1Cells = den1.primitiveFieldRef();
            scalarField& denkt1Cells = denkt1.primitiveFieldRef();
            scalarField& denkt2Cells = denkt2.primitiveFieldRef();
            scalarField& denkr1Cells = denkr1.primitiveFieldRef();
            scalarField& denkr2Cells = denkr2.primitiveFieldRef();

            forAll(species(), speciej)
            {
                if(speciesThermo_[speciej].particleType() > 0)  // Heavy-particle - Heavy-particle collision
                {
                    const volScalarField& pDj = thermo_.composition().pD(speciej);
                    const scalarField& pDjCells = pDj.internalField();
                    
                    if (speciesThermo_[speciei].particleCharge() == 0 or speciesThermo_[speciej].particleCharge() == 0) // Collision involving at least one neutral
                    {
                        forAll(TtCells, celli)
                        {
                            den1Cells[celli] += pDjCells[celli]/(rhoCells[celli]*W(speciej))
                                *collisionIntegralNeutral2(speciei, speciej, TtCells[celli]);
                            
                            denkt1Cells[celli] += asr(speciei, speciej)*den1Cells[celli];
                            denkr1Cells[celli] += pDjCells[celli]/(rhoCells[celli]*W(speciej))
                                *collisionIntegralNeutral1(speciei, speciej, TtCells[celli]);
                        }
                        
                        forAll(pDj.boundaryField(), patchi)
                        {
                            const fvPatchScalarField& pTt = Tt.boundaryField()[patchi];
                            const fvPatchScalarField& prho = rho.boundaryField()[patchi];
                            const fvPatchScalarField& ppDj = pDj.boundaryField()[patchi];
                            fvPatchScalarField& pden1 = den1.boundaryFieldRef()[patchi];
                            fvPatchScalarField& pdenkt1 = denkt1.boundaryFieldRef()[patchi];
                            fvPatchScalarField& pdenkr1 = denkr1.boundaryFieldRef()[patchi];
                            
                            forAll(ppDj, facei)
                            {
                                pden1[facei] += ppDj[facei]/(prho[facei]*W(speciej))
                                    *collisionIntegralNeutral2(speciei, speciej, pTt[facei]);
                                
                                pdenkt1[facei] += asr(speciei, speciej)*pden1[facei];
                                pdenkr1[facei] += ppDj[facei]/(prho[facei]*W(speciej))
                                    *collisionIntegralNeutral1(speciei, speciej, pTt[facei]);
                            }
                        }
                    }
		    else // Collision between two ions
                    {
                        const volScalarField& nDe = thermo_.composition().nD("e-");
                        const scalarField& nDeCells = nDe.internalField();
                        
                        forAll(TtCells, celli)
                        {
                            den1Cells[celli] += pDjCells[celli]/(rhoCells[celli]*W(speciej))
                                *collisionIntegralNonNeutral2(speciei, speciej, TtCells[celli], nDeCells[celli]);
                            
                            denkt1Cells[celli] += asr(speciei, speciej)*den1Cells[celli];
                            denkr1Cells[celli] += pDjCells[celli]/(rhoCells[celli]*W(speciej))
                                *collisionIntegralNonNeutral1(speciei, speciej, TtCells[celli], nDeCells[celli]);
                        }
                        
                        forAll(pDj.boundaryField(), patchi)
                        {
                            const fvPatchScalarField& pTt = Tt.boundaryField()[patchi];
                            const fvPatchScalarField& prho = rho.boundaryField()[patchi];
                            const fvPatchScalarField& ppDj = pDj.boundaryField()[patchi];
                            const fvPatchScalarField& pnDe = nDe.boundaryField()[patchi];
                            fvPatchScalarField& pden1 = den1.boundaryFieldRef()[patchi];
                            fvPatchScalarField& pdenkt1 = denkt1.boundaryFieldRef()[patchi];
                            fvPatchScalarField& pdenkr1 = denkr1.boundaryFieldRef()[patchi];
                            
                            forAll(ppDj, facei)
                            {
                                pden1[facei] += ppDj[facei]/(prho[facei]*W(speciej))
                                    *collisionIntegralNonNeutral2(speciei, speciej, pTt[facei], pnDe[facei]);
                                
                                pdenkt1[facei] += asr(speciei, speciej)*pden1[facei];
                                pdenkr1[facei] += ppDj[facei]/(prho[facei]*W(speciej))
                                    *collisionIntegralNonNeutral1(speciei, speciej, pTt[facei], pnDe[facei]);
                            }
                        }
                    }
                } 
                else // The colliding partner of the heavy-particle is an electron
                {
                    const volScalarField& pDe = thermo_.composition().pD(speciej);
                    const volScalarField& Tve = thermo_.composition().Tv(speciej);
                    const volScalarField& nDe = thermo_.composition().nD(speciej);
                    
                    const scalarField& pDeCells = pDe.internalField();
                    const scalarField& TveCells = Tve.internalField();
                    const scalarField& nDeCells = nDe.internalField();
                    
                    forAll(pDeCells, celli)
                    {
			den1Cells[celli] += pDeCells[celli]/(rhoCells[celli]*W(speciej))
			    *collisionIntegralNonNeutral2(speciei, speciej, TveCells[celli], nDeCells[celli]);

                        denkt2Cells[celli] = 3.54*pDeCells[celli]/(rhoCells[celli]*W(speciej))
                            *collisionIntegralNonNeutral2(speciei, speciej, TveCells[celli], nDeCells[celli]);
                        denkr2Cells[celli] = pDeCells[celli]/(rhoCells[celli]*W(speciej))
                            *collisionIntegralNonNeutral1(speciei, speciej, TveCells[celli], nDeCells[celli]);
                    }
                    
                    forAll(pDe.boundaryField(), patchi)
                    {
                        const fvPatchScalarField& pTve = Tve.boundaryField()[patchi];
                        const fvPatchScalarField& prho = rho.boundaryField()[patchi];
                        const fvPatchScalarField& ppDe = pDe.boundaryField()[patchi];
                        const fvPatchScalarField& pnDe = nDe.boundaryField()[patchi];
			fvPatchScalarField& pden1 = den1.boundaryFieldRef()[patchi];
                        fvPatchScalarField& pdenkt2 = denkt2.boundaryFieldRef()[patchi];
                        fvPatchScalarField& pdenkr2 = denkr2.boundaryFieldRef()[patchi];
                        
                        forAll(ppDe, facei)
                        {
			    pden1[facei] += ppDe[facei]/(prho[facei]*W(speciej))
				*collisionIntegralNonNeutral2(speciei, speciej, pTve[facei], pnDe[facei]);

                            pdenkt2[facei] = 3.54*ppDe[facei]/(prho[facei]*W(speciej))
                                *collisionIntegralNonNeutral2(speciei, speciej, pTve[facei], pnDe[facei]);
                            pdenkr2[facei] = ppDe[facei]/(prho[facei]*W(speciej))
                                *collisionIntegralNonNeutral1(speciei, speciej, pTve[facei], pnDe[facei]);
                        }
                    }
                }
            } // End inner speciej loop

            // Reconstruction for speciei
            forAll(TtCells, celli)
            {    
                muCells[celli] += ms(speciei)*pDCells[celli]/(rhoCells[celli]*W(speciei))/den1Cells[celli];
                
                kappatCells[celli] += pDCells[celli]/(rhoCells[celli]*W(speciei))/(denkt1Cells[celli]+denkt2Cells[celli]);
                if(speciesThermo_[speciei].particleType() == 2 or speciesThermo_[speciei].particleType() == 4)
                {
                    kapparCells[celli] += pDCells[celli]/(rhoCells[celli]*W(speciei))/(denkr1Cells[celli]+denkr2Cells[celli]);
                }    
            }   

            forAll(Tt.boundaryField(), patchi)
            {     
                const fvPatchScalarField& pTt = Tt.boundaryField()[patchi];
                const fvPatchScalarField& prho = rho.boundaryField()[patchi];
                const fvPatchScalarField& ppD = pD.boundaryField()[patchi];
                
                const fvPatchScalarField& pden1 = den1.boundaryField()[patchi];
                const fvPatchScalarField& pdenkt1 = denkt1.boundaryField()[patchi];
                const fvPatchScalarField& pdenkr1 = denkr1.boundaryField()[patchi];
                const fvPatchScalarField& pdenkt2 = denkt2.boundaryField()[patchi];
                const fvPatchScalarField& pdenkr2 = denkr2.boundaryField()[patchi];
                
                fvPatchScalarField& pmu = tmpMu.boundaryFieldRef()[patchi];
                fvPatchScalarField& pkappat = tmpKappat.boundaryFieldRef()[patchi];
                fvPatchScalarField& pkappar = tmpKappar.boundaryFieldRef()[patchi];
                
                forAll(pTt, facei)
                {   
                    pmu[facei] += ms(speciei)*ppD[facei]/(prho[facei]*W(speciei))/pden1[facei];
                    
                    pkappat[facei] += ppD[facei]/(prho[facei]*W(speciei))/(pdenkt1[facei]+pdenkt2[facei]);
		    if(speciesThermo_[speciei].particleType() == 2 or speciesThermo_[speciei].particleType() == 4)
                    {
                        pkappar[facei] += ppD[facei]/(prho[facei]*W(speciei))/(pdenkr1[facei]+pdenkr2[facei]);
                    }
                }  
            }
        }
        else // speciei = electron
        {
            const volScalarField& pD = thermo_.composition().pD(speciei);
	    const volScalarField& Tve = thermo_.composition().Tv(speciei);
	    const volScalarField& nDe = thermo_.composition().nD(speciei);
            volScalarField denEl = pD*0.0;
            
            const scalarField& pDCells = pD.internalField();
	    const scalarField& TveCells = Tve.internalField();
	    const scalarField& nDeCells = nDe.internalField();
            scalarField& denElCells = denEl.primitiveFieldRef();

            forAll(species(), speciej)
            {
		const volScalarField& pDj = thermo_.composition().pD(speciej);
		const scalarField& pDjCells = pDj.internalField();

		if (speciesThermo_[speciej].particleCharge() == 0) // Collision involving at least one neutral
		{
		    forAll(TveCells, celli)
		    {
			denElCells[celli] += pDjCells[celli]/(rhoCells[celli]*W(speciej))
			    *collisionIntegralNeutral2(speciei, speciej, TveCells[celli]);
		    }

		    forAll(pDj.boundaryField(), patchi)
		    {
			const fvPatchScalarField& pTve = Tve.boundaryField()[patchi];
			const fvPatchScalarField& prho = rho.boundaryField()[patchi];
			const fvPatchScalarField& ppDj = pDj.boundaryField()[patchi];
			fvPatchScalarField& pdenEl = denEl.boundaryFieldRef()[patchi];

			forAll(ppDj, facei)
			{
			    pdenEl[facei] += ppDj[facei]/(prho[facei]*W(speciej))
				*collisionIntegralNeutral2(speciei, speciej, pTve[facei]);
			}
		    }
		}
		else
		{
		    forAll(TveCells, celli)
		    {
			denElCells[celli] = pDjCells[celli]/(rhoCells[celli]*W(speciej))
			    *collisionIntegralNonNeutral2(speciei, speciej, TveCells[celli], nDeCells[celli]);
		    }

		    forAll(pDj.boundaryField(), patchi)
		    {
			const fvPatchScalarField& pTve = Tve.boundaryField()[patchi];
			const fvPatchScalarField& prho = rho.boundaryField()[patchi];
			const fvPatchScalarField& ppDj = pDj.boundaryField()[patchi];
			const fvPatchScalarField& pnDe = nDe.boundaryField()[patchi];
			fvPatchScalarField& pdenEl = denEl.boundaryFieldRef()[patchi];
			    
			forAll(ppDj, facei)
			{
			    pdenEl[facei] += ppDj[facei]/(prho[facei]*W(speciej))
				*collisionIntegralNonNeutral2(speciei, speciej, pTve[facei], pnDe[facei]);
			}
		    }
		}
	    }

            // Reconstruction for speciei = electron
            forAll(TveCells, celli)
            {
		if (denElCells[celli] > VSMALL) //ROOTVSMALL
		{
		    muCells[celli] += ms(speciei)*pDCells[celli]/(rhoCells[celli]*W(speciei))/denElCells[celli];
		}
            }   

            forAll(Tve.boundaryField(), patchi)
            {     
                const fvPatchScalarField& pTve = Tve.boundaryField()[patchi];
                const fvPatchScalarField& prho = rho.boundaryField()[patchi];
                const fvPatchScalarField& ppD = pD.boundaryField()[patchi];
                
                const fvPatchScalarField& pdenEl = denEl.boundaryField()[patchi];
                
                fvPatchScalarField& pmu = tmpMu.boundaryFieldRef()[patchi];
                
                forAll(pTve, facei)
                {
		    if (pdenEl[facei] > VSMALL) //ROOTVSMALL
		    {
			pmu[facei] += ms(speciei)*ppD[facei]/(prho[facei]*W(speciei))/pdenEl[facei];
		    }
                }  
            }
        }
    } // End speciei loop 
    
    muMix = tmpMu;
    kappatrMix = kB*(15.0/4.0*tmpKappat+tmpKappar);
    
    const volScalarField Cvvel = thermo_.Cv_vel();
    const scalarField& CvvelCells = Cvvel.internalField();
    
    forAll(rhoCells, celli)
    {
        kappaveCells[celli] = (kB*kapparCells[celli])*CvvelCells[celli]/Ru
            *(1.0e-3*thermo_.composition().molWeightMixture(celli));
        if(CvvelCells[celli] != 0.0)
        {
            alphaveCells[celli] = kappaveCells[celli]/CvvelCells[celli];
        }
    }

    forAll(rho.boundaryField(), patchi)
    {        
        const fvPatchScalarField& pCvvel = Cvvel.boundaryField()[patchi];
        const fvPatchScalarField& pkappar = tmpKappar.boundaryField()[patchi];
        fvPatchScalarField& pkappave = tmpKappave.boundaryFieldRef()[patchi];
        fvPatchScalarField& palphave = tmpAlphave.boundaryFieldRef()[patchi];
        
        forAll(pkappave, facei)
        {
            pkappave[facei] = (kB*pkappar[facei])*pCvvel[facei]/Ru
                *(1.0e-3*thermo_.composition().molWeightMixture(patchi, facei));
            if(pCvvel[facei] != 0.0)
            {
                palphave[facei] = pkappave[facei]/pCvvel[facei];
            }
        }
    }

    kappaveMix = tmpKappave;
    alphatrMix = kappatrMix/thermo_.Cv_t();
    alphaveMix = tmpAlphave;
} 


template<class ThermoType>
void Foam::GuptaMR<ThermoType>::write()
{
    if (writeMuSpecies_)
    {
        forAll(species(), speciei)
        {  
            spmu_[speciei].write(); 
        }      
    } 
    
    if (writeMuMixture_)
    {
        thermo_.mu().write();     
    }
    
    if (writeKappaSpecies_)
    {
        forAll(species(), speciei)
        {  
            spkappatr_[speciei].write(); 
            spkappave_[speciei].write();  
        }      
    } 
    
    if (writeKappaMixture_)
    {
        thermo_.kappatr().write(); 
        thermo_.kappave().write();    
    }      
} 


template<class ThermoType>
bool Foam::GuptaMR<ThermoType>::read()
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
