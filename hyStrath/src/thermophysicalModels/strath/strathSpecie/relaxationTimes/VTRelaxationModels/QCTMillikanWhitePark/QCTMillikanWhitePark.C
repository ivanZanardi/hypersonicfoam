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

#include "QCTMillikanWhitePark.H"
#include "addToRunTimeSelectionTable.H"
#include "MillikanWhitePark.H"

#include "DynamicList.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    namespace VTRelaxationModels
    {
        defineTypeNameAndDebug(QCTMillikanWhitePark, 0);
        addToRunTimeSelectionTable
        (
            VTRelaxationModel,
            QCTMillikanWhitePark, 
            dictionary
        );
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::VTRelaxationModels::QCTMillikanWhitePark::QCTMillikanWhitePark
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
)
:
    VTRelaxationModel(name1, name2, lname1, lname2, dict2T, dictThermoPhy, p, Tt, Tv, nD)
{   
    species1_ = lname1; species2_ = lname2;

    QCTon_ = false;

    word PES(dict2T.subDict("QCTCoefficients").lookup("PESType"));
    if (PES != "Varandas" and PES != "HulbertHirshfelder")
    {
        FatalErrorIn("Foam::VTRelaxationModels::QCTMillikanWhitePark::QCTMillikanWhitePark")
            << "This PESType does not exist. Valid types are 'Varandas' and 'HulbertHirshfelder'." << nl;
	FatalError<< exit(FatalError);
    }

    if(dict2T.subDict("QCTCoefficients").subDict(PES).isDict(name1+"_"+name2) or
	dict2T.subDict("QCTCoefficients").subDict(PES).isDict(name2+"_"+name1))
    {
        QCTon_ = true;
    } 
    
    if(QCTon_)
    {
        word subDictName = word::null;
        
        if (dict2T.subDict("QCTCoefficients").subDict(PES).isDict(name1+"_"+name2))
        {
            subDictName = name1+"_"+name2;
        }
        else
        {
            subDictName = name2+"_"+name1;
        }

        FixedList<scalar,4> defaultList(0.0);
	QCT_ = dict2T.subDict("QCTCoefficients").subDict(PES).lookupOrDefault<FixedList<scalar,4>>(subDictName, defaultList);
    }

    W1_ = readScalar(dictThermoPhy.subDict(name1).subDict("specie").lookup("molWeight"));
    word subDictName = word::null;
    
    if (not VTFullCoeffsForm_)
    {
        const scalar W2 = readScalar(dictThermoPhy.subDict(name2).subDict("specie").lookup("molWeight"));
        DynamicList<scalar> vibData(dictThermoPhy.subDict(name1).subDict("thermodynamics").lookup("vibrationalList"));
        const scalar TH1 = vibData[1];
          
        scalar W12 = (W1_ * W2) / (W1_ + W2);
        A12_ = sqrt(W12) * pow(TH1, 4.0/3.0);
        B12_ = pow(W12, 0.25);
        scalar preAij = 0.0;
        scalar preMij = 0.0;
        
        if (not VTOverwriteDefault_)
        {
            preAij  = 1.16e-3;
            preMij  = 0.015;
            offset_ = 18.42;
            sigma1_ = 1.0e-21;
            sigma2_ = 5.0e4;
        }
        else 
        {
            if (VTSpeciesDependent_ and VTCollidingPartner_)
            {        
                if (dict2T.subDict("ParkCoefficients").isDict(name1+"_"+name2))
                {
                    subDictName = name1+"_"+name2;
                }
                else if (dict2T.subDict("ParkCoefficients").isDict(name2+"_"+name1))
                {
                    subDictName = name2+"_"+name1;
                }
                else if (dict2T.subDict("ParkCoefficients").isDict(name1))
                {
                    subDictName = name1; 
                }
                else
                {
                    subDictName = "allSpecies";
                }    
            }
            else if (VTSpeciesDependent_ and dict2T.subDict("ParkCoefficients").isDict(name1))
            {        
                subDictName = name1;
            } 
            else
            {
                subDictName = "allSpecies";    
            }
            
            preAij = readScalar(dict2T.subDict("ParkCoefficients").subDict(subDictName).lookup("preAij"));
            preMij = readScalar(dict2T.subDict("ParkCoefficients").subDict(subDictName).lookup("preMij"));
            A12_ *= preAij;  
            B12_ *= preMij;
            offset_ = readScalar(dict2T.subDict("ParkCoefficients").subDict(subDictName).lookup("offset"));  
            sigma1_ = readScalar(dict2T.subDict("ParkCoefficients").subDict(subDictName).lookup("sigma1")); 
            sigma2_ = readScalar(dict2T.subDict("ParkCoefficients").subDict(subDictName).lookup("sigma2")); 
        }
    }
    else
    {
        if (not VTOverwriteDefault_)
        {
            A12_  = 221.53;
            B12_  = 0.029;
            offset_ = 18.42;
            sigma1_ = 1.0e-21;
            sigma2_ = 5.0e4;
        }
        else 
        {
            if (VTSpeciesDependent_ and VTCollidingPartner_)
            {        
                if (dict2T.subDict("ParkCoefficients").isDict(name1+"_"+name2))
                {
                    subDictName = name1+"_"+name2;
                }
                else if (dict2T.subDict("ParkCoefficients").isDict(name2+"_"+name1))
                {
                    subDictName = name2+"_"+name1;
                }
                else if (dict2T.subDict("ParkCoefficients").isDict(name1))
                {
                    subDictName = name1;  
                }
                else
                {
                    subDictName = "allSpecies";    
                }    
            }
            else if (VTSpeciesDependent_ and dict2T.subDict("ParkCoefficients").isDict(name1))
            {        
                subDictName = name1; 
            } 
            else
            {
                subDictName = "allSpecies";             
            }
            
            A12_ = readScalar(dict2T.subDict("ParkCoefficients").subDict(subDictName).lookup("Aij"));
            B12_ = readScalar(dict2T.subDict("ParkCoefficients").subDict(subDictName).lookup("Bij"));
            offset_ = readScalar(dict2T.subDict("ParkCoefficients").subDict(subDictName).lookup("offset"));  
            sigma1_ = readScalar(dict2T.subDict("ParkCoefficients").subDict(subDictName).lookup("sigma1")); 
            sigma2_ = readScalar(dict2T.subDict("ParkCoefficients").subDict(subDictName).lookup("sigma2")); 
        }
    }
}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

Foam::tmp<Foam::volScalarField>
Foam::VTRelaxationModels::QCTMillikanWhitePark::tauVT() const
{
    const fvMesh& mesh = this->Tt_.mesh();

    tmp<volScalarField> ttauVT
    (
        new volScalarField
        (
            IOobject
            (
                "tauVT_" + name1_ + "_" + name2_,
                mesh.time().timeName(),
                mesh,
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            mesh,
            dimensionSet(0, 0, 1, 0, 0)
        )
    );

    volScalarField& tauVT = ttauVT.ref();

    volScalarField nDcol = this->nD_[species1_];
    if(species1_ != species2_)
    {
        nDcol += this->nD_[species2_];
    }
    
    forAll(this->Tt_, celli)
    {
	if(QCTon_)
	{
	    scalar x = this->Tt_[celli]/1000;
	    tauVT[celli] = 1.01325e5 / this->p_[celli] * (QCT_[0]*pow(x,3)+QCT_[1]*pow(x,2)+QCT_[2]*x+QCT_[3]) * pow(10, -8);
	}
	else
	{
            tauVT[celli] =
         	1.01325e5 / this->p_[celli] * exp(A12_*(pow(this->Tt_[celli], -1.0/3.0) - B12_) - offset_)
              + 1.0/(sqrt(8.0*constant::physicoChemical::R.value()*1000.0*this->Tt_[celli]/
                  (constant::mathematical::pi*W1_)) * sigma1_*pow(sigma2_/this->Tt_[celli], 2.0) *max(nDcol[celli], Foam::SMALL));
	}        
    }
    

    forAll(this->Tt_.boundaryField(), patchi)
    {
        const fvPatchScalarField& pTt = this->Tt_.boundaryField()[patchi];
        const fvPatchScalarField& pp = this->p_.boundaryField()[patchi];
        const fvPatchScalarField& pnDcol = nDcol.boundaryField()[patchi];
        fvPatchScalarField& ptauVT = tauVT.boundaryFieldRef()[patchi];

	forAll(pTt, facei)
	{
	    if(QCTon_)
	    {
		scalar x = pTt[facei]/1000;
		ptauVT[facei] = 1.01325e5 / pp[facei] * (QCT_[0]*pow(x,3)+QCT_[1]*pow(x,2)+QCT_[2]*x+QCT_[3]) * pow(10, -8);
	    }
	    else
	    {
		ptauVT[facei] =
		    1.01325e5 / pp[facei] * exp(A12_*(pow(pTt[facei], -1.0/3.0) - B12_) - offset_)
		  + 1.0/(sqrt(8.0*constant::physicoChemical::R.value()*1000.0*pTt[facei]/
		      (constant::mathematical::pi*W1_)) * sigma1_*pow(sigma2_/pTt[facei], 2.0) * max(pnDcol[facei], Foam::SMALL));
	    }
        }
    }

    return ttauVT;
}


Foam::tmp<Foam::scalarField> Foam::VTRelaxationModels::QCTMillikanWhitePark::tauVT
(
    const label patchi,
    const scalarField& p,
    const scalarField& Tt,
    const PtrList<scalarField>& Tv,
    const PtrList<scalarField>& nD
) const
{
    tmp<scalarField> ttauVT(new scalarField(Tt.size()));
    scalarField& tauVT = ttauVT.ref();
    
    scalarField nDcol = nD[species1_];
    if(species1_ != species2_)
    {
        nDcol += nD[species2_];
    }
    
    forAll(Tt, facei)
    {
	if(QCTon_)
	{
	    scalar x = Tt[facei]/1000;
	    tauVT[facei] = 1.01325e5 / p[facei] * (QCT_[0]*pow(x,3)+QCT_[1]*pow(x,2)+QCT_[2]*x+QCT_[3]) * pow(10, -8);
	}
	else
	{
	    tauVT[facei] =
		1.01325e5 / p[facei] * exp(A12_*(pow(Tt[facei], -1.0/3.0) - B12_) - offset_)
	      + 1.0/(sqrt(8.0*constant::physicoChemical::R.value()*1000.0*Tt[facei]/
		  (constant::mathematical::pi*W1_)) * sigma1_*pow(sigma2_/Tt[facei], 2.0) * max(nDcol[facei],Foam::SMALL));
	}          
    }

    return ttauVT;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
