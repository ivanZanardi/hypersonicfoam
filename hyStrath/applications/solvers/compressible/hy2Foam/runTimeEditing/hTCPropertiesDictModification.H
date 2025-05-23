
if(noIteration == 1)
{
    if(reaction->found("applyChanges")) reaction->set<bool>("applyChanges", false);
    else reaction->add<bool>("applyChanges", false);
    
    if(reaction->found("applyChangesAtWriteTime")) reaction->set<bool>("applyChangesAtWriteTime", false);
    else reaction->add<bool>("applyChangesAtWriteTime", false);
    
    if(reaction->found("applyChangesAtWriteTimeAndWait")) reaction->set<bool>("applyChangesAtWriteTimeAndWait", false);
    else reaction->add<bool>("applyChangesAtWriteTimeAndWait", false);
}
    
if(reaction->lookupOrDefault<bool>("applyChanges", false))
{
    // --- Print the results before updating the thermo package
    // Will be used for re-start
    #include "write/writeEssentials.H"
    
    // --- End the bool run(...) function and restart the simulation
    return true;
}

