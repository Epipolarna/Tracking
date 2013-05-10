function imfwrite(B, filename, format)
%IMFWRITE Write imf image file.
%
%  imfwrite(B, filename, format)
%  B - real or complex 2D matrix
%  filename - Filename of image file (inclusive full path and extension!)
%  format - imf file format: u8, s8, s16, p8, p16, c8, c16
%
%  IMFWRITE writes the contents of a real or cartesian matlab double matrix
%  into an imf image file.
%  The values in B are clamped to [-1..1] for s8, s16, c8, c16 types,
%  the magnitude is clamped to [0..1] for p8 and p16 image types,
%  and the values in B are clamped to [0..1] for u8 image types.
%
%  Author: Johan Wiklund, jowi@isy.liu.se
%
error(nargchk(3,3,nargin))

[fid, message] = fopen(filename, 'w', 'ieee-be');
if fid == -1
  disp(message)
end

id = 3924623360;      % 0xE9ED0000
cnt = fwrite(fid, id, 'uint32');
if cnt ~= 1
  disp('IMFWRITE: Error in writing magic number')
  return
end

off = 512;
cnt = fwrite(fid, off, 'uint32');
if cnt ~= 1
  disp('IMFWRITE: Error in writing data offset')
  return
end

[hgh wid] = size(B);

switch format
  case 'p16'
    len = wid * hgh * 4;
    typ = 1;
    X = real(B);
    Y = imag(B);
    [A M] = cart2pol(X, Y);
    M = M * 32767.0;
    M(M > 32767) = 32767.0;
    A = (32767.0 / pi) * A;
    B(:,1:2:wid*2) = M;
    B(:,2:2:wid*2) = A;
  
  case 's16'
    if ~isreal(B)
      error(sprintf('Matrix must be real for s16 format'));
    end
    len = wid * hgh * 2;
    typ = 2;
    B = 32767.0 * B;
    B(B > 32767) = 32767.0;
    B(B < -32768) = -32768.0;
  
  case 'c16'
    len = wid * hgh * 4;
    typ = 3;
    X = real(B);
    Y = imag(B);
    B(:,1:2:wid*2) = X;
    B(:,2:2:wid*2) = Y;
    B = 32767.0 * B;
    B(B > 32767) = 32767.0;
    B(B < -32768) = -32768.0;
    
  case 'p8'
    len = wid * hgh * 2;
    typ = 4;
    X = real(B);
    Y = imag(B);
    [A M] = cart2pol(X, Y);
    M = M * 255.0;
    M(M > 255) = 255.0;
    A = (255.0 / (2 * pi)) * A;
    B(:,1:2:wid*2) = M;
    B(:,2:2:wid*2) = A;
  
  case 'u8'
    if ~isreal(B)
      error(sprintf('Matrix must be real for u8 format'));
    end
    pad = bitand(wid,1);
    len = (wid + pad) * hgh;
    typ = 5;
    if pad ~= 0
      B(:,(wid+pad)) = zeros(size(B,1),1);
    end
    B = 255.0 * B;
    B(B > 255) = 255.0;
    B(B < 0) = 0.0;

  case 's8'
    if ~isreal(B)
      error(sprintf('Matrix must be real for s8 format'));
    end
    pad = bitand(wid,1);
    len = (wid + pad) * hgh;
    typ = 6;
    if pad ~= 0
      B(:,(wid+pad)) = zeros(size(B,1),1);
    end
    B = 127.0 * B;
    B(B > 127) = 127.0;
    B(B < -128) = -128.0;
  
  case 'c8'
    len = wid * hgh * 2;
    typ = 7;
    X = real(B);
    Y = imag(B);
    B(:,1:2:wid*2) = X;
    B(:,2:2:wid*2) = Y;
    B = 127.0 * B;
    B(B > 127) = 127.0;
    B(B < -128) = -128.0;

  otherwise
    error(sprintf('Unrecognized format "%s"', format));
    
end

% Transpose image to get correct orientation
B = B.';

cnt = fwrite(fid, len, 'uint32');
if cnt ~= 1
  disp('IMFWRITE: Error in writing data length')
  return
end

cnt = fwrite(fid, hgh, 'uint16');
if cnt ~= 1
  disp('IMFWRITE: Error in writing image height')
  return
end

cnt = fwrite(fid, wid, 'uint16');
if cnt ~= 1
  disp('IMFWRITE: Error in writing image width')
  return
end

cnt = fwrite(fid, typ, 'uint16');
if cnt ~= 1
  disp('IMFWRITE: Error in writing image format')
  return
end

%status = fseek(fid, off, 'bof');
%if status == -1
%  disp('IMFWRITE: Failed in seek in image file')
%  ferror(fid)
%  return
%end
cnt = fwrite(fid, zeros(494,1), 'uint8');
if cnt ~= 494
  disp('IMFWRITE: Failed writing header padding')
  ferror(fid)
  return
end

switch format
  case 'p16'
    cnt = fwrite(fid, B, 'uint16');
    if cnt ~= (len / 2)
      disp('IMFWRITE: Failed writing image data')
      ferror(fid)
      return
    end
  
  case 's16'
    cnt = fwrite(fid, B, 'int16');
    if cnt ~= (len / 2)
      disp('IMFWRITE: Failed writing image data')
      ferror(fid)
      return
    end
  
  case 'c16'
    cnt = fwrite(fid, B, 'int16');
    if cnt ~= (len / 2)
      disp('IMFWRITE: Failed writing image data')
      ferror(fid)
      return
    end
  
  case 'p8'
    cnt = fwrite(fid, B, 'uint8');
    if cnt ~= len
      disp('IMFWRITE: Failed writing image data')
      ferror(fid)
      return
    end
  
  case 'u8'
    cnt = fwrite(fid, B, 'uint8');
    if cnt ~= len
      disp('IMFWRITE: Failed writing image data')
      ferror(fid)
      return
    end
    
  case 's8'
    cnt = fwrite(fid, B, 'int8');
    if cnt ~= len
      disp('IMFWRITE: Failed writing image data')
      ferror(fid)
      return
    end
  
  case 'c8'
    cnt = fwrite(fid, B, 'int8');
    if cnt ~= len
      disp('IMFWRITE: Failed writing image data')
      ferror(fid)
      return
    end
  
  otherwise
    error(sprintf('Unrecognized format "%s"', format));
    
end
