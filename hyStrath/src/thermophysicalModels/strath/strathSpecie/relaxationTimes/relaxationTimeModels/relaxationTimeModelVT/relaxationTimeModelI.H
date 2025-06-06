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

inline const Foam::volScalarField&
Foam::relaxationTimeModel::tauVTij(label i, label j) const
{
    return tauVTijModel_().tauVT(i,j);
}


inline const Foam::volScalarField&
Foam::relaxationTimeModel::tauVT(label speciei) const
{
    return tauVT_[speciei];
}


inline const Foam::volScalarField&
Foam::relaxationTimeModel::tauVT(const word& specieName) const
{
    return tauVT_[solvedVibEqSpecies()[specieName]];
}


inline const Foam::volScalarField&
Foam::relaxationTimeModel::QVT(label speciei) const
{
    return QVT_[speciei];
}


inline const Foam::volScalarField&
Foam::relaxationTimeModel::QVT(const word& specieName) const
{
    return QVT_[solvedVibEqSpecies()[specieName]];
}


inline Foam::volScalarField&
Foam::relaxationTimeModel::tauVT(label speciei)
{
    return tauVT_[speciei];
}


inline Foam::volScalarField&
Foam::relaxationTimeModel::tauVT(const word& specieName)
{
    return tauVT_[solvedVibEqSpecies()[specieName]];
}


inline Foam::volScalarField&
Foam::relaxationTimeModel::QVT(label speciei)
{
    return QVT_[speciei];
}


inline Foam::volScalarField&
Foam::relaxationTimeModel::QVT(const word& specieName)
{
    return QVT_[solvedVibEqSpecies()[specieName]];
}


/*inline const Foam::volScalarField& // TODO ONGOING WORK
Foam::relaxationTimeModel::QVT_mode(label speciei, label vibMode) const
{
    return QVTmode_[speciei][vibMode];
}*/


inline const Foam::speciesTable&
Foam::relaxationTimeModel::species() const
{
    return thermo_.composition().species();
}

inline const Foam::speciesTable&
Foam::relaxationTimeModel::solvedVibEqSpecies() const // NEW VINCENT 05/08/2016
{
    return thermo_.composition().solvedVibEqSpecies();
}

// ************************************************************************* //
