clear all
close all
clc

x = lower(input('Have you already cleaned the data? [Y/n] ', 's'));
x = strtok(x(find(isletter(x), 1)));
while 1
    switch x
       case 'y'
          break
       case 'n'
          !./clean
          break
       otherwise
          x = lower(input('Please answer yes or no: ','s'));
          x = strtok(x(find(isletter(x), 1)));
    end
end

% Load data
rho = 9.78449e-6;
U = 4440;
n = 5;
for i = 1:n
    dir = ["TC"+i];
    cd(dir)
    dat{i,1} = load('frictionCoefficient_obstacle.raw');
    dat{i,2} = load('StantonNo_obstacle.raw');
    dat{i,3} = load('nD_O2_obstacle.raw');
    if isfile('nD_O_obstacle.raw')
        dat{i,4} = load('nD_O_obstacle.raw');
    else
        dat{i,4} = [];
    end
    dat{i,5} = load('kappatr_obstacle.raw');
    dat{i,6} = load('kappave_obstacle.raw');
    dat{i,7} = load('wallHeatFlux_obstacle.raw');
    cd ../
    
    y_old{i}= dat{i,1}(:,2);
    Cf_old{i} = dat{i,1}(:,4);
    St_old{i} = dat{i,2}(:,4);
    nD_O2_old{i} = dat{i,3}(:,4);
    if ~isempty(dat{i,4})
        nD_O_old{i} = dat{i,4}(:,4);
    else
        nD_O_old{i} = [];
    end
    kappatr_old{i} = dat{i,5}(:,4);
    kappave_old{i} = dat{i,6}(:,4);
    WH_old{i} = dat{i,7}(:,4);
    
    y{i} = sort(y_old{i});
    Cf{i} = [];
    St{i} = [];
    nD_O2{i} = [];
    nD_O{i} = [];
    kappatr{i} = [];
    kappave{i} = [];
    WH{i} = [];
    for l=1:length(y_old{i})
        q = find(y_old{i} == y{i}(l,1));
        Cf{i} = [Cf{i}; Cf_old{i}(q,1)];
        St{i} = [St{i}; St_old{i}(q,1)];
        nD_O2{i} = [nD_O2{i}; nD_O2_old{i}(q,1)];
        if ~isempty(dat{i,4})
            nD_O{i} = [nD_O{i}; nD_O_old{i}(q,1)];
        else
            nD_O{i} = [];
        end
        kappatr{i} = [kappatr{i}; kappatr_old{i}(q,1)];
        kappave{i} = [kappave{i}; kappave_old{i}(q,1)];
        WH{i} = [WH{i}; WH_old{i}(q,1)];
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

m = min(abs(y{1}-0.9));
q = find((abs(y{1}-0.9)-m) < 1e-8);
fileID = fopen('data.txt','w');

% Friction ----------------------------------------------------------------
h(1) = figure;
hold all
fprintf(fileID,'C_f values at %0.8f m high:\n',y{1}(q,1));
for i=1:n
    plot(y{i},Cf{i},line2{2},'Color',col{i},...
        'DisplayName',"C_"+i,'LineWidth',1);
    fprintf(fileID,'- test case C%0.0f: %0.7f\n',i,Cf{i}(q,1));
end
xlim([0 1])
legend('-DynamicLegend','Location','NorthWest');
ylabel('C_f')
xlabel('Baffle height  [m]')
grid on
%set(gca, 'Xdir', 'reverse')
%set(gca, 'YAxisLocation', 'Right')
set(gca,'FontName','Helvetica');
set(gcf,'color','w');
set(gcf,'position',[x0,y0,width,height]);
export_fig Friction -pdf -q101 -nocrop

% Stanton -----------------------------------------------------------------
h(2) = figure;
hold all
fprintf(fileID,'\nSt values at %0.8f m high:\n',y{1}(q,1));
for i=1:n
    plot(y{i},WH{i}./(0.5*rho*U^3),line2{2},'Color',col{i},...
        'DisplayName',"C_"+i,'LineWidth',1);
    fprintf(fileID,'- test case C%0.0f: %0.7f\n',i,WH{i}(q,1)/(0.5*rho*U^3));
end
xlim([0 1])
legend('-DynamicLegend','Location','NorthWest');
ylabel('St')
xlabel('Baffle height  [m]')
grid on
%set(gca, 'Xdir', 'reverse')
%set(gca, 'YAxisLocation', 'Right')
set(gca,'FontName','Helvetica');
set(gcf,'color','w');
set(gcf,'position',[x0,y0,width,height]);
export_fig Stanton -pdf -q101 -nocrop

% Thermal Conductivity --------------------------------------------------
h(3) = figure;
hold all
for i=[2,4]
    plot(y{i},kappatr{i},line2{2},'Color',col{i},...
        'DisplayName',"k_{tr}  of  C_"+i,'LineWidth',1);
    plot(y{i},kappave{i},line2{1},'Color',col{i},...
        'DisplayName',"k_{ve} of  C_"+i,'LineWidth',1);
end
xlim([0 1])
legend('-DynamicLegend','Location','West');
xlabel('Baffle height  [m]')
ylabel('k  [W/(m K))]')
grid on
set(gca,'FontName','Helvetica');
set(gcf,'color','w');
set(gcf,'position',[x0,y0,width,height]);
export_fig thermalConductivityNReac -pdf -q101 -nocrop

h(4) = figure;
hold all
for i=1:n
    if ~isempty(nD_O{i})
        plot(y{i},kappatr{i},line2{2},'Color',col{i},...
            'DisplayName',"k_{tr}  of  C_"+i,'LineWidth',1);
        plot(y{i},kappave{i},line2{1},'Color',col{i},...
            'DisplayName',"k_{ve} of  C_"+i,'LineWidth',1);
    end
end
xlim([0 1])
legend('-DynamicLegend','Location','West');
xlabel('Baffle height  [m]')
ylabel('k  [W/(m K))]')
grid on
set(gca,'FontName','Helvetica');
set(gcf,'color','w');
set(gcf,'position',[x0,y0,width,height]);
export_fig thermalConductivityReac -pdf -q101 -nocrop

fprintf(fileID,'\nk_tr values at stagnation point:\n');
for i=1:n
    fprintf(fileID,'- test case C%0.0f: %0.7f W/(m K)\n',i,kappatr{i}(1,1));
end

fprintf(fileID,'\nk_ve values at stagnation point:\n');
for i=1:n
    fprintf(fileID,'- test case C%0.0f: %0.7f W/(m K)\n',i,kappave{i}(1,1));
end

% Number Density ----------------------------------------------------------
h(5) = figure;
for i=1:n
    if isempty(nD_O{i})
        plot(y{i},nD_O2{i},line2{2},'Color',col{i},...
            'DisplayName',"n_{O_2} of  C_"+i,'LineWidth',1);
        hold on
    end
end
xlim([0 1])
legend('-DynamicLegend','Location','SouthWest');
xlabel('Baffle height  [m]')
ylabel('n  [m^{-3}]')
grid on
set(gca,'FontName','Helvetica');
set(gcf,'color','w');
set(gcf,'position',[x0,y0,width,height]);
export_fig numbDenNReac -pdf -q101 -nocrop

h(6) = figure;
for i=1:n
    if ~isempty(nD_O{i})
        plot(y{i},nD_O2{i},line2{2},'Color',col{i},...
            'DisplayName',"n_{O_2} of  C_"+i,'LineWidth',1);
        hold on
        plot(y{i},nD_O{i},line2{1},'Color',col{i},...
            'DisplayName',"n_{O}  of  C_"+i,'LineWidth',1);
    end
end
xlim([0 1])
legend('-DynamicLegend','Location','SouthWest');
xlabel('Baffle height  [m]')
ylabel('n  [m^{-3}]')
grid on
set(gca,'FontName','Helvetica');
set(gcf,'color','w');
set(gcf,'position',[x0,y0,width,height]);
export_fig numbDenReac -pdf -q101 -nocrop

h(7) = figure;
plot(y{i},nD_O2{5}./nD_O2{4},line2{2},'Color',col{1},'LineWidth',1);
xlim([0 1])
xlabel('Baffle height  [m]')
ylabel('n_{ratio}')
grid on
set(gca,'FontName','Helvetica');
set(gcf,'color','w');
set(gcf,'position',[x0,y0,width,height]);
export_fig numbDenRatio -pdf -q101 -nocrop

%{
% Stanton -----------------------------------------------------------------
h(7) = figure;
hold all
fprintf(fileID,'\nSt values at %0.8f m high:\n',y{1}(q,1));
for i=1:n
    plot(y{i},St{i},line2{2},'Color',col{i},...
        'DisplayName',"C_"+i,'LineWidth',1);
    fprintf(fileID,'- test case C%0.0f: %0.7f\n',i,St{i}(q,1));
end
xlim([0 1])
legend('-DynamicLegend','Location','NorthWest');
ylabel('St')
xlabel('Baffle height  [m]')
grid on
%set(gca, 'Xdir', 'reverse')
%set(gca, 'YAxisLocation', 'Right')
set(gca,'FontName','Helvetica');
set(gcf,'color','w');
set(gcf,'position',[x0,y0,width,height]);
export_fig Stanton -pdf -q101 -nocrop
%}

fclose(fileID);

cd ../
