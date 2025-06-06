/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011-2013 OpenFOAM Foundation
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

#include "noMHD.H"
#include "physicoChemicalConstants.H"
#include "fvMesh.H"
#include "Time.H"
#include "volFields.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    namespace mhd
    {
        defineTypeNameAndDebug(noMHD, 0);
        addToMhdRunTimeSelectionTables(noMHD);



// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

noMHD::noMHD(const rho2ReactionThermo& thermo)
:
    mhdModel(thermo)
{}


noMHD::noMHD
(
    const dictionary& dict,
    const rho2ReactionThermo& thermo
)
:
    mhdModel(thermo)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

noMHD::~noMHD()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool noMHD::read()
{
    return mhdModel::read();
}


tmp<volVectorField> noMHD::F(const volVectorField& U) const
{
    return tmp<volVectorField>
    (
        new volVectorField
        (
            IOobject
            (
                "F",
                mesh_.time().timeName(),
                mesh_,
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            mesh_,
            dimensionedVector
            (
                "F",
                dimensionSet(1, -2, -2, 0, 0, 0, 0),
                vector(0.0, 0.0, 0.0)
            )
        )
    );
}


tmp<volScalarField> noMHD::Q(const volVectorField& U) const
{
    return tmp<volScalarField>
    (
        new volScalarField
        (
            IOobject
            (
                "Rp",
                mesh_.time().timeName(),
                mesh_,
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            mesh_,
            dimensionedScalar
            (
                "Q",
                dimensionSet(1, -1, -3, 0, 0, 0, 0),
                0.0
            )
        )
    );
}

/*
Foam::tmp<Foam::DimensionedField<Foam::scalar, Foam::volMesh> >
Foam::radiation::noRadiation::Ru() const
{
    return tmp<DimensionedField<scalar, volMesh> >
    (
        new DimensionedField<scalar, volMesh>
        (
            IOobject
            (
                "Ru",
                mesh_.time().timeName(),
                mesh_,
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            mesh_,
            dimensionedScalar
            (
                "Ru", dimMass/dimLength/pow3(dimTime), 0.0
            )
        )
    );
}*/

} // End namespace mhd
} // End namespace FOAM

// ************************************************************************* //
