DROP TABLE IF EXISTS pagadas, pendientes;

CREATE TABLE pagadas(cod_serv INTEGER, tipo INTEGER, compr BIGINT, monto BIGINT, dig_verif INTEGER, prefijo INTEGER, numero INTEGER, medidor BIGINT, abonado BIGINT, transaccion INTEGER, fecha_hora BIGINT,  usuario TEXT, vencimiento INTEGER);

CREATE TABLE pendientes(cod_serv INTEGER,tipo INTEGER,compr BIGINT,monto BIGINT,dig_verif INTEGER,prefijo INTEGER,numero INTEGER ,medidor BIGINT, abonado BIGINT, id INTEGER,vencimiento INTEGER);

INSERT INTO pendientes VALUES (001,002,12345678912,000000100000,7,0644,9876543,987654321012345,987654321,1,20131212);
