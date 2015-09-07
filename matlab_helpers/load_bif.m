function A = load_bif(filename)

% read array dimensions
fileId = fopen(filename, 'r');
dim = fread(fileId, [1 3], 'uint64');
fclose(fileId);
dim = [dim(2), dim(1), dim(3)];

% read array data
offset = 24; % first 24 bytes contain array dimensions
format = 'bip'; % pixel-interleaved
endian = 'ieee-le'; % little-endian
A = multibandread(filename, dim, 'double', offset, format, endian);

% reshape the array to match MATLAB conventions
A = permute(A, [2 1 3]);

end