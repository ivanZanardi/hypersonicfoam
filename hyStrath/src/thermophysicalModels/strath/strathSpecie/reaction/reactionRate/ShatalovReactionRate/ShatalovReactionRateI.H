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

\*---------------------------------------------------------------------------*/

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

inline Foam::ShatalovReactionRate::ShatalovReactionRate
(
    const scalar A,
    const scalar beta,
    const scalar Th,
    const scalar Ta
)
:
    A_(A),
    beta_(beta),
    Th_(Th),
    Ta_(Ta)
{}


inline Foam::ShatalovReactionRate::ShatalovReactionRate
(
    const speciesTable&,
    Istream& is
)
:
    A_(readScalar(is.readBegin("ShatalovReactionRate(Istream&)"))),
    beta_(readScalar(is)),
    Th_(readScalar(is)),
    Ta_(readScalar(is))
{
    is.readEnd("ShatalovReactionRate(Istream&)");
}


inline Foam::ShatalovReactionRate::ShatalovReactionRate
(
    const speciesTable&,
    const dictionary& dict
)
:
    A_(readScalar(dict.lookup("A"))),
    beta_(readScalar(dict.lookup("beta"))),
    Th_(readScalar(dict.lookup("Th"))),
    Ta_(readScalar(dict.lookup("Ta")))
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

inline Foam::scalar Foam::ShatalovReactionRate::operator()
(
    const scalar p,
    const scalar T,
    const scalarField&
) const
{
    scalar ak = A_;

    if (mag(beta_) > VSMALL)
    {
        ak *= pow(T, beta_);
    }

    if (mag(Th_) > VSMALL)
    {
        ak *= (1.0 - exp(-Th_/T));
    }

    if (mag(Ta_) > VSMALL)
    {
        ak *= exp(-Ta_/T);
    }

    return ak;
}


inline void Foam::ShatalovReactionRate::write(Ostream& os) const
{
    os.writeKeyword("A") << A_ << token::END_STATEMENT << nl;
    os.writeKeyword("beta") << beta_ << token::END_STATEMENT << nl;
    os.writeKeyword("Th") << Th_ << token::END_STATEMENT << nl;
    os.writeKeyword("Ta") << Ta_ << token::END_STATEMENT << nl;
}


inline Foam::Ostream& Foam::operator<<
(
    Ostream& os,
    const ShatalovReactionRate& arr
)
{
    os  << token::BEGIN_LIST
        << arr.A_ << token::SPACE << arr.beta_ << token::SPACE << arr.Th_ << token::SPACE << arr.Ta_
        << token::END_LIST;
    return os;
}


// ************************************************************************* //
