function imf = imfread(filename)
%IMFREAD Read imf image file.
%
%  imf = imfread(filename)
%  filename - Filename of image file (inclusive full path and extension!)
%
%  IMFREAD reads a imf image file and stores the image in a real or
%  cartesian matlab double matrix.
%  The pixel values are normalized to [-1..1] for s8, s16, c8, c16, p8 and
%  p16 image types, and to [0..1] for u8 image types.
%
%  Author: Johan Wiklund, jowi@isy.liu.se
%
error(nargchk(1,1,nargin))

[fid, message] = fopen(filename, 'r', 'ieee-be');
if fid == -1
  disp(message)
end

id  = fread(fid, 1, 'uint32');
if id ~= 3924623360      % 0xE9ED0000
  disp('IMFREAD: wrong magic number')
  fclose(fid);
  return
end

off = fread(fid, 1, 'uint32');
len = fread(fid, 1, 'uint32');
hgh = fread(fid, 1, 'uint16');
wid = fread(fid, 1, 'uint16');
typ = fread(fid, 1, 'uint16');

status = fseek(fid, off, 'bof');
if status == -1
  disp('IMFREAD: Failed reading image file')
  ferror(fid)
  fclose(fid);
  return
end

switch typ
  case 1,                 % p16
    imfsize = hgh * wid * 2;
    [imf cnt] = fread(fid, imfsize, 'uint16');
    if cnt ~= imfsize
      disp('IMFREAD: Failed reading image file')
      ferror(fid)
      fclose(fid);
      return
    end
    mscale = 1.0 / 32767.0;
    ascale = pi / 32767.0;
    M = mscale * imf(1:2:imfsize);
    A = ascale * imf(2:2:imfsize);
    M = reshape(M, [wid hgh]);
    A = reshape(A, [wid hgh]);
    [X, Y] = pol2cart(A, M);
    imf = X + i*Y;
  case 2,                 % s16
    imfsize = hgh * wid;
    [imf cnt] = fread(fid, imfsize, 'int16');
    if cnt ~= imfsize
      disp('IMFREAD: Failed reading image file')
      ferror(fid)
      fclose(fid);
      return
    end
    mscale = 1.0 / 32767.0;
    imf = mscale * imf;
    imf = reshape(imf, [wid hgh]);
  case 3,                 % c16
    imfsize = hgh * wid * 2;
    [imf cnt] = fread(fid, imfsize, 'int16');
    if cnt ~= imfsize
      disp('IMFREAD: Failed reading image file')
      ferror(fid)
      fclose(fid);
      return
    end
    mscale = 1.0 / 32767.0;
    imf = mscale * imf;
    imf = imf(1:2:imfsize) + i*imf(2:2:imfsize);
    imf = reshape(imf, [wid hgh]);
  case 4,                 % p8
    imfsize = hgh * wid * 2;
    [imf cnt] = fread(fid, imfsize, 'uint8');
    if cnt ~= imfsize
      disp('IMFREAD: Failed reading image file')
      ferror(fid)
      fclose(fid);
      return
    end
    mscale = 1.0 / 255.0;
    ascale = 2 * pi / 255.0;
    M = mscale * imf(1:2:imfsize);
    A = ascale * imf(2:2:imfsize);
    M = reshape(M, [wid hgh]);
    A = reshape(A, [wid hgh]);
    [X, Y] = pol2cart(A, M);
    imf = X + i*Y;
  case 5,                 % u8
    skip = bitand(wid,1);
    imfsize = hgh * (wid+skip);
    [imf cnt] = fread(fid, imfsize, 'uint8');
    if cnt ~= imfsize
      disp('IMFREAD: Failed reading image file')
      ferror(fid)
      fclose(fid);
      return
    end
    mscale = 1.0 / 255.0;
    imf = mscale * imf;
    imf = reshape(imf, [(wid+skip) hgh]);
    if skip ~= 0
      imf = imf(1:wid,:);
    end
  case 6,                 % s8
    skip = bitand(wid,1);
    imfsize = hgh * (wid+skip);
    [imf cnt] = fread(fid, imfsize, 'int8');
    if cnt ~= imfsize
      disp('IMFREAD: Failed reading image file')
      ferror(fid)
      fclose(fid);
      return
    end
    mscale = 1.0 / 127.0;
    imf = mscale * imf;
    imf = reshape(imf, [(wid+skip) hgh]);
    if skip ~= 0
      imf = imf(1:wid,:);
    end
  case 7,                 % c8
    imfsize = hgh * wid * 2;
    [imf cnt] = fread(fid, imfsize, 'int8');
    if cnt ~= imfsize
      disp('IMFREAD: Failed reading image file')
      ferror(fid)
      fclose(fid);
      return
    end
    mscale = 1.0 / 127.0;
    imf = mscale * imf;
    imf = imf(1:2:imfsize) + i*imf(2:2:imfsize);
    imf = reshape(imf, [wid hgh]);
  otherwise,
    disp('IMFREAD: unsupported image type'),
    fclose(fid);
    return
end

imf = transpose(imf);
fclose(fid);
