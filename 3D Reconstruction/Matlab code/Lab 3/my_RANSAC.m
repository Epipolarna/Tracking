%
% Returns
%   F - The fundamental matrix
%   IP - Interest Poitns
%   C - Corresponding points
%
%
%
%

function [ F IP1 IP2 C ind1 ind2 bestP1 bestP2 ] = my_RANSAC( Image1, Image2 )
    
tic
    F = zeros(3,3);
    C = [];
    
    featureSize = 7;
    supressionSize = 10;
    thresholdDivider = 200;
    
    ImGray1 = rgb2gray(Image1);
    ImGray2 = rgb2gray(Image2);

    % Calculate interest points
    IP1 = harris(ImGray1, featureSize, 1);
    IP2 = harris(ImGray2, featureSize, 1);
    
    figure(123);
    subplot(1,2,1); imshow(IP1);
    subplot(1,2,2); imshow(IP2);
    
    IP1 = non_max_suppression(IP1, supressionSize);
    IP2 = non_max_suppression(IP2, supressionSize);
    
    figure(124);
    subplot(1,2,1); imshow(IP1);
    subplot(1,2,2); imshow(IP2);
        
    [I J] = find(IP1 > max(max(IP1))/thresholdDivider);
    IP1 = [J I];
    [I J] = find(IP2 > max(max(IP2))/thresholdDivider);
    IP2 = [J I];
    
    figure(123123);
     imshow(Image2);hold on; plot(J,I,'xr'); hold off;
    toc
    
    tic
    % Putative Correspondence
    c_matrix = zeros(length(IP1), length(IP2));
    regionSize = 14;
    
    roi1 = cut_out_rois(ImGray1, IP1(:,1), IP1(:,2), regionSize);
    roi2 = cut_out_rois(ImGray2, IP2(:,1), IP2(:,2), regionSize);
    
    for p1 = 1:length(IP1)
        for p2 = 1:length(IP2)
            c_matrix(p1,p2) = norm(IP1(p1,:)-IP2(p2,:)) + norm(roi1(:,p1)-roi2(:,p2));
        end
    end
    [C, ind1, ind2] = joint_min(c_matrix);
    
    %ind1 = ind1(1:length(ind1)/2);
    %ind2 = ind2(1:length(ind2)/2);
    %C = C(1:length(C)/2);
    
    toc
    tic
    % RANSAC
    bestConsensus = -1;
    bestF = [];
    bestP1 = [];
    bestP2 = [];
    for iter = 1:10000
        rIndex = randperm(length(C),8);
        rPoints1 = IP1(ind1(rIndex), :);
        rPoints2 = IP2(ind2(rIndex), :);
        F = fmatrix_stls(rPoints1', rPoints2');
        r = fmatrix_residuals(F, IP1(ind1,:)', IP2(ind2,:)');
        
        consensus = (sum(sqrt(r(1,:).^2+r(2,:).^2) < 3))/length(ind1);
        if(consensus > bestConsensus)
            
            sum(sqrt(r(1,:).^2+r(2,:).^2) < 3)
            
            bestConsensus = consensus
            bestF = F
            bestP1 = ind1(rIndex);
            bestP2 = ind2(rIndex);
        end
    end
    F = bestF;
    
    toc
end

