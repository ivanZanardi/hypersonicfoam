/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011-2012 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Class
    Foam::ShatalovReactionRate

Description
    Shatalov (1999) reaction rate given by:

        k = A * T^beta * (1 - exp(-Th/T)) * exp(-Ta/T)

SourceFiles
    ShatalovReactionRateI.H

Journal article
    Dissociation rate constants of diatomic molecules under thermal
	equilibrium conditions
    Ibragimova, L. B. and Smekhov, G. D. and Shatalov, O. P.
    Fluid Dynamics 34, 1999
    doi: 10.1007/BF02698767    

\*---------------------------------------------------------------------------*/

#ifndef ShatalovReactionRate_H
#define ShatalovReactionRate_H

#include "scalarField.H"
#include "typeInfo.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                    Class ShatalovReactionRate Declaration
\*---------------------------------------------------------------------------*/

class ShatalovReactionRate
{
    // Private data

        scalar A_;
        scalar beta_;
        scalar Th_;
        scalar Ta_;


public:

    // Constructors

        //- Construct from components
        inline ShatalovReactionRate
        (
            const scalar A,
            const scalar beta,
            const scalar Th,
            const scalar Ta
        );

        //- Construct from Istream
        inline ShatalovReactionRate
        (
            const speciesTable& species,
            Istream& is
        );

        //- Construct from dictionary
        inline ShatalovReactionRate
        (
            const speciesTable& species,
            const dictionary& dict
        );


    // Member Functions

        //- Return the type name
        static word type()
        {
            return "Shatalov";
        }

        inline scalar operator()
        (
            const scalar p,
            const scalar T,
            const scalarField& c
        ) const;

        //- Write to stream
        inline void write(Ostream& os) const;


    // Ostream Operator

        inline friend Ostream& operator<<
        (
            Ostream&,
            const ShatalovReactionRate&
        );
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "ShatalovReactionRateI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
