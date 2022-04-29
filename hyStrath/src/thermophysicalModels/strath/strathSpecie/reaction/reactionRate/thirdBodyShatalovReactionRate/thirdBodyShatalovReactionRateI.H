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

\*---------------------------------------------------------------------------*/

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

inline Foam::thirdBodyShatalovReactionRate::thirdBodyShatalovReactionRate
(
    const scalar A,
    const scalar beta,
    const scalar Th,
    const scalar Ta,
    const thirdBodyEfficiencies& tbes
)
:
    ShatalovReactionRate(A, beta, Th, Ta),
    thirdBodyEfficiencies_(tbes)
{}


inline Foam::thirdBodyShatalovReactionRate::thirdBodyShatalovReactionRate
(
    const speciesTable& species,
    const dictionary& dict
)
:
    ShatalovReactionRate
    (
        species,
        dict
    ),
    thirdBodyEfficiencies_(species, dict)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

inline Foam::scalar Foam::thirdBodyShatalovReactionRate::operator()
(
    const scalar p,
    const scalar T,
    const scalarField& c
) const
{
    return
        thirdBodyEfficiencies_.M(c)
       *ShatalovReactionRate::operator()(p, T, c);
}


inline void Foam::thirdBodyShatalovReactionRate::write(Ostream& os) const
{
    ShatalovReactionRate::write(os);
    thirdBodyEfficiencies_.write(os);
}


inline Foam::Ostream& Foam::operator<<
(
    Ostream& os,
    const thirdBodyShatalovReactionRate& arr
)
{
    arr.write(os);
    return os;
}


// ************************************************************************* //
