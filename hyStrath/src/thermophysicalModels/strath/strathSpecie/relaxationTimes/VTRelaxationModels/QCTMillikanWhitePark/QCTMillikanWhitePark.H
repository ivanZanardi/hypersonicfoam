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
    Foam::VTRelaxationModels::QCTMillikanWhitePark

Description
    Millikan-White-Park V-T Relaxation model with correlation for specific
    molecular interactions derived from QCT (quasi classical trajectory).
    See:
	"Modeling Fidelity for Oxygen Nonequilibrium Thermochemistry in
	Reflected Shock Tube Flows".
	Kevin Neitzel, Daniil Andrienko, and Iain D Boyd.
	https://doi.org/10.2514/6.2015-2509
    
SourceFiles
    QCTMillikanWhitePark.C

\*---------------------------------------------------------------------------*/

#ifndef QCTMillikanWhitePark_H
#define QCTMillikanWhitePark_H

#include "VTRelaxationModel.H"
#include "dimensionedScalar.H"
#include "volFields.H"

#include "fundamentalConstants.H"
#include "physicoChemicalConstants.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace VTRelaxationModels
{

/*---------------------------------------------------------------------------*\
                           Class QCTMillikanWhitePark Declaration
\*---------------------------------------------------------------------------*/

class QCTMillikanWhitePark
:
    public VTRelaxationModel
{
    // Private data

        // Coefficient
        scalar species1_, species2_;
        
        // QCT or MWP VT energy exchange?
        bool QCTon_;
        
        // Coefficients
        scalar A12_, B12_; 
        
        // Molecular weight, species i [g/mol]
        scalar W1_;   
        
        // Offset term in the exponential
        scalar offset_;   
        
        // Coefficients related to Park's correction
        scalar sigma1_, sigma2_;  
        
        // Coefficients related to QCTVarandas's empirical correlation
        FixedList<scalar, 4> QCT_;


public:

    //- Runtime type information
    TypeName("QCTMillikanWhitePark");


    // Constructors

        //- construct from components
        QCTMillikanWhitePark
        (
            const word& name1,
            const word& name2,
            const label& lname1,
            const label& lname2,
            const dictionary& dict2T,
            const dictionary& dictThermoPhy,
            const volScalarField& p,
            const volScalarField& Tt,
            const PtrList<volScalarField>& Tv,
            const PtrList<volScalarField>& nD
        );


    // Destructor

        ~QCTMillikanWhitePark()
        {}


    // Member Functions

        //- V-T relaxation time [s]
        virtual tmp<volScalarField> tauVT() const;
         
        //- V-T relaxation time for patch [s]
        virtual tmp<scalarField> tauVT
        (
            const label patchi,
            const scalarField& p,
            const scalarField& Tt,
            const PtrList<scalarField>& Tv,
            const PtrList<scalarField>& nD 
        ) const;
        
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace VTRelaxationModels
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
