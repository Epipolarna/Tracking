%%
initcourse TSBB15

%% 4

%Test images
Im1 = imread('img1.png');
Im2 = imread('img2.png');

[F IP1 IP2 C ind1 ind2 bestP1 bestP2] = my_RANSAC(Im1, Im2);

figure(1); clf;
subplot(1,2,1); imshow(Im1);
subplot(1,2,2); imshow(Im2);

figure(3); clf;
show_corresp(Im1, Im2, IP1, IP2, [ind1(1:5:end) ind2(1:5:end)])

figure(5); clf;
show_corresp(Im1, Im2, IP1, IP2, [bestP1 bestP2]); title('bla');

figure(4); clf;
plot_eplines(F, [IP1(ind1) IP2(ind2)], [0 size(Im1,1) 0 size(Im1,2)]);
