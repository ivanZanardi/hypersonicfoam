clear all
close all
clc

% Load data
n = 5;
for i = 1:n
    dir = ["TC"+i];
    cd(dir)
    switch i
        case 3
            dat{i,1} = load('line_Cpt_Cpvel_KnGLL_Kn_ov_Mach_O_O2_Tt_nD_O_nD_O2_p_rho.xy');
        case 4
            dat{i,1} = load('line_Cpt_Cpvel_KnGLL_Kn_ov_Mach_O2_Tt_Tv_nD_O2_p_rho_tauVT_O2.xy');
        case {5, 6}
            dat{i,1} = load('line_Cpt_Cpvel_KnGLL_Kn_ov_Mach_O_O2_Tt_Tv_nD_O_nD_O2_p_rho_tauVT_O2.xy');
        otherwise
            dat{i,1} = load('line_Cpt_Cpvel_KnGLL_Kn_ov_Mach_O2_Tt_nD_O2_p_rho.xy');
    end
    dat{i,2} = load('line_U_grad(p).xy');
    cd ../
end
L = 1.5; % Length of stagnation line
for i = 1:n
    x{i} = dat{i,1}(:,1)-L;
    m = min(abs(x{i}+1.1));
    q(i) = find(abs(abs(x{i}+1.1)-m) < 1e-8);
    
    x{i} = [ -1.1; (dat{i,1}((q(i)+1):end,1)-L) ];
    Cpt{i} = dat{i,1}((q(i):end),2);
    Cpvel{i} = dat{i,1}((q(i):end),3);
    Mach{i} = dat{i,1}((q(i):end),6);
    gradP{i} = dat{i,2}((q(i):end),5);
    switch i
        case 3
            O{i} = dat{i,1}((q(i):end),7);
            O2{i} = dat{i,1}((q(i):end),8);
            Tt{i} = dat{i,1}((q(i):end),9);
            nD_O{i} = dat{i,1}((q(i):end),10);
            nD_O2{i} = dat{i,1}((q(i):end),11);
            p{i} = dat{i,1}((q(i):end),12);
            rho{i} = dat{i,1}((q(i):end),13);
        case 4
            Tt{i} = dat{i,1}((q(i):end),8);
            Tv{i} = dat{i,1}((q(i):end),9);
            nD_O2{i} = dat{i,1}((q(i):end),10);
            p{i} = dat{i,1}((q(i):end),11);
            rho{i} = dat{i,1}((q(i):end),12);
        case {5, 6}
            O{i} = dat{i,1}((q(i):end),7);
            O2{i} = dat{i,1}((q(i):end),8);
            Tt{i} = dat{i,1}((q(i):end),9);
            Tv{i} = dat{i,1}((q(i):end),10);
            nD_O{i} = dat{i,1}((q(i):end),11);
            nD_O2{i} = dat{i,1}((q(i):end),12);
            p{i} = dat{i,1}((q(i):end),13);
            rho{i} = dat{i,1}((q(i):end),14);
        otherwise
            Tt{i} = dat{i,1}((q(i):end),8);
            nD_O2{i} = dat{i,1}((q(i):end),9);
            p{i} = dat{i,1}((q(i):end),10);
            rho{i} = dat{i,1}((q(i):end),11);
    end
end

% PLOTTING ----------------------------------------------------------------
mkdir figures
cd figures
col = {[0, 0.4470, 0.7410]; [0.8500, 0.3250, 0.0980];...
    [0.4660, 0.6740, 0.1880]; [0.4940, 0.1840, 0.5560];...
    [0.9290, 0.6940, 0.1250]; [0.3010 0.7450 0.9330]};
%line1 = {'-->','--^','--*','--x','--d','->','-^','-*','-x','-d'};
line2 = {'--','-'};
%N = 100; % Number of plot markers
x0=500;
y0=400;
width=700;
height=400;

% Temperature -------------------------------------------------------------
h(1) = figure;
hold all
for i=1:n
    if isempty(O{i})
        if ~isempty(Tv{i})
            %a = round(length(x{i})/N); % Distance between plot markers
            plot(x{i},Tt{i},line2{2},'Color',col{i},...
                'DisplayName',"T_{tr}  of  C_"+i,'LineWidth',1);
            %'MarkerIndices',(a*i+1):(a*n):length(x{i}),...
            plot(x{i},Tv{i},line2{1},'Color',col{i},...
                'DisplayName',"T_{ve} of  C_"+i,'LineWidth',1);
        else
            plot(x{i},Tt{i},line2{2},'Color',col{i},...
                'DisplayName',"T of  C_"+i,'LineWidth',1);
        end
    end
end
xlim([-1.1 0])
legend('-DynamicLegend','Location','South');
xlabel('x  [m]')
ylabel('T  [K]')
grid on
set(gca,'FontName','Helvetica');
set(gcf,'color','w');
set(gcf,'position',[x0,y0,width,height]);
export_fig temperatureNReac -pdf -q101 -nocrop

h(2) = figure;
hold all
for i=1:n
    if ~isempty(O{i})
        if ~isempty(Tv{i})
            plot(x{i},Tt{i},line2{2},'Color',col{i},...
                'DisplayName',"T_{tr}  of  C_"+i,'LineWidth',1);
            plot(x{i},Tv{i},line2{1},'Color',col{i},...
                'DisplayName',"T_{ve} of  C_"+i,'LineWidth',1);
        else
            plot(x{i},Tt{i},line2{2},'Color',col{i},...
                'DisplayName',"T of  C_"+i,'LineWidth',1);
        end
    end
end
xlim([-1.1 0])
legend('-DynamicLegend','Location','Best');
xlabel('x  [m]')
ylabel('T  [K]')
grid on
set(gca,'FontName','Helvetica');
set(gcf,'color','w');
set(gcf,'position',[x0,y0,width,height]);
export_fig temperatureReac -pdf -q101 -nocrop

% Level of noneq. ---------------------------------------------------------
fileID = fopen('data.txt','w');
fprintf(fileID,'Nonequilibrium region length:\n');
for i=1:n
    if ~isempty(Tv{i})
        delT{i}=(Tt{i}-Tv{i})./Tv{i};
        
        M = max(delT{i});
        w = find(abs(delT{i}-M) < 1e-8);
        p1 = find(abs(delT{i}(1:w,1))<0.01,1,'last');
        p2 = find(abs(delT{i}(w:end,1))<0.01,1,'first');
        delX(i) = x{i}(p2+w,1)-x{i}(p1,1);
        fprintf(fileID,'- test case C%0.0f: %0.8f m\n',i,delX(i));
    end
end

h(3) = figure;
hold all
for i=1:n
    if ~isempty(Tv{i})
        plot(x{i},delT{i},line2{2},'Color',col{i},...
            'DisplayName',"C_"+i,'LineWidth',1);
    end
end
xlim([-1.1 0])
legend('-DynamicLegend','Location','NorthEast');
xlabel('x  [m]')
ylabel('\delta_{eq}')
grid on
set(gca,'FontName','Helvetica');
set(gcf,'color','w');
set(gcf,'position',[x0,y0,width,height]);
export_fig levelNoneq -pdf -q101 -nocrop

% Specific Heat -----------------------------------------------------------
h(4) = figure;
hold all
for i=[2,4]
    plot(x{i},Cpt{i},line2{2},'Color',col{i},...
        'DisplayName',"C_{p,tr}  of  C_"+i,'LineWidth',1);
    plot(x{i},Cpvel{i},line2{1},'Color',col{i},...
        'DisplayName',"C_{p,ve} of  C_"+i,'LineWidth',1);
end
xlim([-1.1 0])
legend('-DynamicLegend','Location','West');
xlabel('x  [m]')
ylabel('C_p  [J/(kg K))]')
grid on
set(gca,'FontName','Helvetica');
set(gcf,'color','w');
set(gcf,'position',[x0,y0,width,height]);
export_fig specificHeatNReac -pdf -q101 -nocrop

h(5) = figure;
hold all
for i=1:n
    if ~isempty(O{i})
        plot(x{i},Cpt{i},line2{2},'Color',col{i},...
            'DisplayName',"C_{p,tr}  of  C_"+i,'LineWidth',1);
        plot(x{i},Cpvel{i},line2{1},'Color',col{i},...
            'DisplayName',"C_{p,ve} of  C_"+i,'LineWidth',1);
    end
end
xlim([-1.1 0])
legend('-DynamicLegend','Location','West');
xlabel('x  [m]')
ylabel('C_p  [J/(kg K))]')
grid on
set(gca,'FontName','Helvetica');
set(gcf,'color','w');
set(gcf,'position',[x0,y0,width,height]);
export_fig specificHeatReac -pdf -q101 -nocrop

% Mach --------------------------------------------------------------------
h(6) = figure;
hold all
for i=1:n
    plot(x{i},Mach{i},line2{2},'Color',col{i},...
        'DisplayName',"C_"+i,'LineWidth',1);
end
xlim([-1.1 0])
legend('-DynamicLegend','Location','NorthEast');
xlabel('x  [m]')
ylabel('Ma')
grid on
set(gca,'FontName','Helvetica');
set(gcf,'color','w');
set(gcf,'position',[x0,y0,width,height]);
export_fig mach -pdf -q101 -nocrop

h(7) = figure;
hold all
for i=1:n
    plot(x{i},Mach{i},line2{2},'Color',col{i},...
        'DisplayName',"C_"+i,'LineWidth',1);
end
xlim([-1.1 0])
ylim([0 1])
legend('-DynamicLegend','Location','NorthEast');
xlabel('x  [m]')
ylabel('Ma')
grid on
set(gca,'FontName','Helvetica');
set(gcf,'color','w');
set(gcf,'position',[x0,y0,width,height]);
export_fig subsonicRegion -pdf -q101 -nocrop

% Composition -------------------------------------------------------------
h(8) = figure;
hold all
for i=1:n
    if ~isempty(O{i})
        plot(x{i},O2{i},line2{2},'Color',col{i},...
            'DisplayName',"Y_{O_2} of  C_"+i,'LineWidth',1);
        plot(x{i},O{i},line2{1},'Color',col{i},...
            'DisplayName',"Y_O  of  C_"+i,'LineWidth',1);
    end
end
xlim([-1.1 0])
legend('-DynamicLegend','Location','West');
xlabel('x  [m]')
ylabel('Y')
grid on
set(gca,'FontName','Helvetica');
set(gcf,'color','w');
set(gcf,'position',[x0,y0,width,height]);
export_fig composition -pdf -q101 -nocrop

% Number Density ----------------------------------------------------------
h(9) = figure;
for i=1:n
    if isempty(O{i})
        semilogy(x{i},nD_O2{i},line2{2},'Color',col{i},...
            'DisplayName',"n_{O_2} of  C_"+i,'LineWidth',1);
        hold on
    end
end
xlim([-1.1 0])
legend('-DynamicLegend','Location','NorthWest');
xlabel('x  [m]')
ylabel('n  [m^{-3}]')
grid on
set(gca,'FontName','Helvetica');
set(gcf,'color','w');
set(gcf,'position',[x0,y0,width,height]);
export_fig numbDenNReac -pdf -q101 -nocrop

h(10) = figure;
for i=1:n
    if ~isempty(O{i})
        semilogy(x{i},nD_O2{i},line2{2},'Color',col{i},...
            'DisplayName',"n_{O_2} of  C_"+i,'LineWidth',1);
        hold on
        semilogy(x{i},nD_O{i},line2{1},'Color',col{i},...
            'DisplayName',"n_{O}  of  C_"+i,'LineWidth',1);
    end
end
xlim([-1.1 0])
ylim([1e18 2e22])
legend('-DynamicLegend','Location','NorthWest');
xlabel('x  [m]')
ylabel('n  [m^{-3}]')
grid on
set(gca,'FontName','Helvetica');
set(gcf,'color','w');
set(gcf,'position',[x0,y0,width,height]);
export_fig numbDenReac -pdf -q101 -nocrop

h(11) = figure;
for i=1:n
    if ~isempty(O{i})
        plot(x{i},nD_O{i}./nD_O2{i},line2{2},'Color',col{i},...
            'DisplayName',"C_"+i,'LineWidth',1);
        hold on
    end
end
xlim([-1.1 0])
legend('-DynamicLegend','Location','NorthWest');
xlabel('x  [m]')
ylabel('n_{O}/n_{O_2}')
grid on
set(gca,'FontName','Helvetica');
set(gcf,'color','w');
set(gcf,'position',[x0,y0,width,height]);
export_fig numbDenRatio -pdf -q101 -nocrop

% Pressure ----------------------------------------------------------------
h(12) = figure;
hold all
for i=1:n
    plot(x{i},p{i},line2{2},'Color',col{i},...
        'DisplayName',"C_"+i,'LineWidth',1);
end
xlim([-1.1 0])
legend('-DynamicLegend','Location','SouthEast');
xlabel('x  [m]')
ylabel('p  [Pa]')
grid on
set(gca,'FontName','Helvetica');
set(gcf,'color','w');
set(gcf,'position',[x0,y0,width,height]);
export_fig pressure -pdf -q101 -nocrop

% Density ----------------------------------------------------------------
h(13) = figure;
for i=1:n
    semilogy(x{i},rho{i},line2{2},'Color',col{i},...
        'DisplayName',"C_"+i,'LineWidth',1);
    hold on
end
xlim([-1.1 0])
ylim([9e-6 2e-3])
legend('-DynamicLegend','Location','NorthWest');
xlabel('x  [m]')
ylabel('\rho  [kg/m^3]')
grid on
set(gca,'FontName','Helvetica');
set(gcf,'color','w');
set(gcf,'position',[x0,y0,width,height]);
export_fig density -pdf -q101 -nocrop

% Stand-off ---------------------------------------------------------------
h(14) = figure;
fprintf(fileID,'\nStand-off distance:\n');
hold all
for i=1:n
    m = min(abs(x{i}+0.1));
    w = find(abs(abs(x{i}+0.1)-m) < 1e-8);
    plot(x{i}(1:w,1),gradP{i}(1:w,1),line2{2},'Color',col{i},...
        'DisplayName',"C_"+i,'LineWidth',1);
    
    M = max(gradP{i});
    w = find(abs(gradP{i}-M) < 1e-8);
    fprintf(fileID,'- test case C%0.0f: %0.8f m\n',i,x{i}(w,1));
end
xlim([-1.1 -0.1])
legend('-DynamicLegend','Location','NorthEast');
xlabel('x  [m]')
ylabel(['(\nabla' 'p)_x  [Pa/m]'])
grid on
set(gca,'FontName','Helvetica');
set(gcf,'color','w');
set(gcf,'position',[x0,y0,width,height]);
export_fig standOff -pdf -q101 -nocrop

fclose(fileID);

cd ../
