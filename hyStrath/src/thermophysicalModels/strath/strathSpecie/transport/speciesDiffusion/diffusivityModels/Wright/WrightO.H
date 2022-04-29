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

Class
    Foam::binaryDiffusivityModels::WrightO

Description
    WrightO binary diffusivity model.
    
    - From:
    DOI: 102514/1.16713 (Wright, Bose, Palmer, Levin: Aug. 2005)
    "Recommended Collision Integrals for Transport Property Computations,
     Part 1: Air Species"

    - As cited by:
    AIAA 2007-605 (Hash et al.: Jan. 2007)
    "FIRE II Calculations for Hypersonic nonequilibrium Aerothermo-
     dynamics Code verification: DPLR, LAURA, and US3D"
    
SourceFiles
    WrightO.C

\*---------------------------------------------------------------------------*/

#ifndef WrightO_H
#define WrightO_H

#include "binaryDiffusivityModel.H"
#include "dimensionedScalar.H"
#include "volFields.H"

#include "constants.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace binaryDiffusivityModels
{

/*---------------------------------------------------------------------------*\
                           Class WrightO Declaration
\*---------------------------------------------------------------------------*/

class WrightO
:
    public binaryDiffusivityModel
{
    // Private data

        //- Model coefficients
        FixedList<scalar, 4> piOmega11Neu_;
        FixedList<FixedList<scalar, 3>, 2> piOmega11NonNeu_; 
        
        //- Molecular weights
        const scalar W1_, W2_;
        
        //- Fundamental constants
        const scalar pi, kB, Runi, eCGS;
        
        //- Constant in the neutral collision term
        const scalar constantFactorInCollisionTerm_;
        
        //- Potential type: attractive or repulsive
        label potentialType_;

        
    // Private member functions
        
        //- Collision term 1
        inline scalar collisionTerm1(const scalar T, const scalar nDe = 0) const
        {
            if(T != 0.0)
            {
                if(this->collisionType_ < 2)
                {
                    return constantFactorInCollisionTerm_/sqrt(T)
			    *piOmega11Neutral(T);
                }
                else if(nDe > VSMALL)
                {
		    if(this->collisionType_ == 3)
		    {
			return constantFactorInCollisionTerm_/sqrt(T)
				*piOmega11NonNeutral(1, T, nDe);
		    }
		    else
		    {
			return constantFactorInCollisionTerm_/sqrt(T)
				*piOmega11NonNeutral(0, T, nDe);
		    }
                }
            }
            
            return VSMALL;
        }
        
        //- Collision cross-section (1,1) for neutral interactions
        //  in Angstrom^2
        inline scalar piOmega11Neutral(const scalar T) const 
        {
            return piOmega11Neu_[3]
                *pow
                (
                    T, 
                    piOmega11Neu_[0]*sqr(log(T)) 
                  + piOmega11Neu_[1]*log(T) + piOmega11Neu_[2]
                );
        }
        
        //- Collision cross-section (1,1) for non-neutral interactions
        //  in Angstrom^2
	inline scalar piOmega11NonNeutral(label potential, const scalar T, const scalar nDe) const
        {
            scalar kBCGS(kB*1.0e7); // SI in Joule, CGS in erg
            scalar nDeCGS(max(nDe, 1.0)*1.0e6); // SI in m^-3, CGS in cm^-3
						// 'max' is used if nDe = 0 to avoid division by
						// 0, set to very small value (1)
            scalar DebyeLength = sqrt(kBCGS*T/(4.0*pi*nDeCGS*sqr(eCGS)));
            scalar Tstar = DebyeLength*kBCGS*T/sqr(eCGS);
            
            return 5.0e15*pi*sqr(DebyeLength/T)*log(piOmega11NonNeu_[potential][2]*Tstar
                *(1.0-piOmega11NonNeu_[potential][1]*exp(-piOmega11NonNeu_[potential][0]*Tstar)) + 1.0);
        } 


public:

    //- Runtime type information
    TypeName("WrightO");


    // Constructors

        //- construct from components
        WrightO
        (
            const word& name1,
            const word& name2,
            const dictionary& dictThermo,
            const dictionary& dictTransport,
            const volScalarField& p,
            const volScalarField& pe,
            const volScalarField& nDe,
            const volScalarField& T
        );


    // Destructor

        ~WrightO()
        {}


    // Member Functions

        //- Binary diffusion coefficient [m^2/s]
        virtual tmp<volScalarField> D() const;
         
        //- Binary diffusion coefficient for patch [m^2/s]
        virtual tmp<scalarField> D
        (
            const scalarField& p,
            const scalarField& T,
            const label patchi
        ) const;
        
        //- Binary diffusion coefficient for patch [m^2/s]
        virtual tmp<scalarField> D
        (
            const scalarField& p,
            const scalarField& pe,
            const scalarField& nDe,
            const scalarField& T,
            const label patchi
        ) const;
         
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace binaryDiffusivityModels
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
