function Iout = interpolation_function(Iin, d);

[R,K] = size(Iin);

Iout = interp2(1:K, 1:R, Iin, (1:K)+d(1), (1:R)'+d(2), '*linear');