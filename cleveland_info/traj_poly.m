% specify if you want just flexion or both flexion and extension
% spec = 0 for flexion only -- total time will be t
% spec = 1 for both flexion and extension -- total time will be double t
spec = 1; 

% file declarations
traj_fileID = fopen('traj_data.txt', 'w'); % angular trajectory data
stim_fileID = fopen('open_stim.txt', 'w'); % open loop pulse width values

% trajectory parameters
theta1 = 20; % start angle in degrees
theta2 = 90; % end angle in degrees
t = 5; % total time of flexion in seconds
tstep = 0.1; % time step in seconds

% stimulation parameters
stim = [8 10 12 13]; % pulse width values that produce the corresponding angles in following vector
angle = [55 75 87 90]; % must include theta2
minstim = 6; % minimum pulsewidth that produces motion

% fit end conditions to fifth order polynomial
A = [       0        0       0   0 0 1;
          t^5      t^4     t^3 t^2 t 1;
            0        0       0   0 1 0;
      5*(t^4)  4*(t^3) 3*(t^2) 2*t 1 0;
            0        0       0   2 0 0;
     20*(t^3) 12*(t^2)     6*t   2 0 0];
B = [theta1;
     theta2;
          0;
          0;
          0;
          0];
X = A \ B;
% fifth order polynomial coefficients
a = X(1); b = X(2); c = X(3); d = X(4); e = X(5); f = X(6);

% populate theta vector
count = 0;
for i = 0:tstep:t
    
    count = count + 1;
    theta(count) = (a*(i^5)) + (b*(i^4)) + (c*(i^3)) + (d*(i^2)) + (e*i) + f;
    
end

% print theta to text file
if spec == 0
    len = length(theta);
    for j = 1:length(theta)
        
        fprintf(traj_fileID, '%2.2f\n', theta(j)); 
        
    end
elseif spec == 1
    theta = [theta flip(theta)];
    len = length(theta) / 2;
    for j = 1:length(theta)
        
        fprintf(traj_fileID, '%2.2f\n', theta(j)); 
        
    end
else
    printf('ERROR: spec must be equal to either 0 or 1');
end


% find open loop control
time = 0:tstep:t;
t_save = 0;

for m = 1:length(stim)
    
    for k = 2:len
        
        if (theta(k-1)<angle(m) && theta(k)>angle(m)) || theta(k)==angle(m)
            
            t_save = [t_save interp1([theta(k-1) theta(k)], [time(k-1) time(k)], angle(m))];
            
        end
        
    end
    
end


stim = [minstim stim];
count = 0;

% populate stim_final vector using interpolation
for n = time
    
    count = count + 1;
    stim_final(count) = interp1(t_save, stim, n);
    
end

% print open loop pulse width values to text file
if spec == 0
    for r = 1:length(stim_final)
        
        fprintf(stim_fileID, '%2.2f\n', stim_final(r)); 
        
    end
elseif spec == 1
    stim_final = [stim_final flip(stim_final)];
    for r = 1:length(stim_final)
        
        fprintf(stim_fileID, '%2.2f\n', stim_final(r)); 
        
    end
else
    printf('ERROR: spec must be equal to either 0 or 1');
end

% close data files
fclose(traj_fileID);
fclose(stim_fileID);

