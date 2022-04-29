reaction->correct();
volScalarField Yt(0.0*Y[0]);

forAll(Y, i)
{
    if (i != inertIndex && composition.active(i))
    {
	volScalarField& Yi = Y[i];

	//- KNP interpolation
	surfaceScalarField Yi_pos("Yi_pos", interpolate(Yi, pos, Yi.name()));
	surfaceScalarField Yi_neg("Yi_neg", interpolate(Yi, neg, Yi.name()));

	//- Calculate fluxes for the convective term
	surfaceScalarField phiYi
	(
    	    "phiYi",
    	    (aphiv_pos*rho_pos*Yi_pos + aphiv_neg*rho_neg*Yi_neg)
	);

	fvScalarMatrix YiEqn
	(
	    fvm::ddt(rho, Yi)
	  + fvc::div(phiYi)
	 ==
	    reaction->R(Yi)
	);

	fvOptions.constrain(YiEqn);
        YiEqn.solve("Yi");
	fvOptions.correct(Yi);

	Yi.correctBoundaryConditions();

	if (!inviscid)
	{
	    solve
	    (
	        fvm::ddt(rho, Yi) - fvc::ddt(rho, Yi)
	      - fvm::laplacian(muEff, Yi)
	    );
	}

	Yi.max(0.0);
	Yt += Yi;

    }
}

Y[inertIndex] = scalar(1) - Yt;
Y[inertIndex].max(0.0);

