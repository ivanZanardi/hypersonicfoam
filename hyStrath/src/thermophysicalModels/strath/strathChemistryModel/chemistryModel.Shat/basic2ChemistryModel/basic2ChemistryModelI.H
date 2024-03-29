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

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

inline const Foam::fvMesh& Foam::basic2ChemistryModel::mesh() const
{
    return mesh_;
}


inline Foam::Switch Foam::basic2ChemistryModel::chemistry() const
{
    return chemistry_;
}


inline const Foam::DimensionedField<Foam::scalar, Foam::volMesh>&
Foam::basic2ChemistryModel::deltaTChem() const
{
    return deltaTChem_;
}


inline Foam::DimensionedField<Foam::scalar, Foam::volMesh>&
Foam::basic2ChemistryModel::deltaTChem()
{
    return deltaTChem_;
}


inline Foam::Switch Foam::basic2ChemistryModel::modifiedTemperature() const
{
    return modifiedTemperature_;
}

// ************************************************************************* //
