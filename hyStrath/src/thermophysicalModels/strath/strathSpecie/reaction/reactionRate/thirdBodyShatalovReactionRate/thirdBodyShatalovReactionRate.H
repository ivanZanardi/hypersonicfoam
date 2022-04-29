/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011-2017 OpenFOAM Foundation
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
    Foam::thirdBodyShatalovReactionRate

Description
    Shatalov reaction rate enhanced by third-body interation.

SourceFiles
    thirdBodyShatalovReactionRateI.H

\*---------------------------------------------------------------------------*/

#ifndef thirdBodyShatalovReactionRate_H
#define thirdBodyShatalovReactionRate_H

#include "ShatalovReactionRate.H"
#include "thirdBodyEfficiencies.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward declaration of friend functions and operators

class thirdBodyShatalovReactionRate;

Ostream& operator<<(Ostream&, const thirdBodyShatalovReactionRate&);


/*---------------------------------------------------------------------------*\
              Class thirdBodyShatalovReactionRate Declaration
\*---------------------------------------------------------------------------*/

class thirdBodyShatalovReactionRate
:
    ShatalovReactionRate
{
    // Private data

        thirdBodyEfficiencies thirdBodyEfficiencies_;


public:

    // Constructors

        //- Construct from components
        inline thirdBodyShatalovReactionRate
        (
            const scalar A,
            const scalar beta,
            const scalar Th,
            const scalar Ta,
            const thirdBodyEfficiencies& tbes
        );

        //- Construct from dictionary
        inline thirdBodyShatalovReactionRate
        (
            const speciesTable& species,
            const dictionary& dict
        );


    // Member Functions

        //- Return the type name
        static word type()
        {
            return "thirdBodyShatalov";
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
            const thirdBodyShatalovReactionRate&
        );
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "thirdBodyShatalovReactionRateI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
