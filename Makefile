EXTENSION   = projet
MODULES 	= projet
DATA        = projet--1.0.sql projet.control

PG_CONFIG ?= pg_config

PGXS = $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

