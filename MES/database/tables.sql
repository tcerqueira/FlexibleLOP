-- ==== Document for the sql tables ====

-- Transform Order
CREATE TABLE IF NOT EXISTS TransformOrder (
    id_number INTEGER,
    piece_initial INTEGER,
    piece_final INTEGER,
    total_amount INTEGER,
    done_amount INTEGER,
    doing_amount INTEGER,
    sent_at INTEGER,
    received_at INTEGER,
    max_delay INTEGER,
    penalty_per_day INTEGER,
    started_at INTEGER,
    finished_at INTEGER,
    penalty INTEGER,
    PRIMARY KEY (id_number),
    CHECK (penalty_per_day>=0), CHECK (penalty_per_day<=1000),
    CHECK (piece_initial>=1), CHECK (piece_initial<=9),
    CHECK (piece_final>=1), CHECK (piece_final<=9),
    CHECK (piece_initial <= piece_final)
);

-- Unload Order
CREATE TABLE IF NOT EXISTS UnloadOrder (
    id_number INTEGER,
    piece_type INTEGER,
    destination INTEGER,
    quantity INTEGER,
    PRIMARY KEY (id_number),
    CHECK (piece_type>=1), CHECK (piece_type<=9)
);

-- Storage Order
CREATE TABLE IF NOT EXISTS PieceStorage (
    piece_type INTEGER,
    amount INTEGER,
    PRIMARY KEY (piece_type),
    CHECK (piece_type>=1), CHECK (piece_type<=9),
    CHECK (amount>=0)
);

-- Machine
CREATE TABLE IF NOT EXISTS Machine (
    id_mac INTEGER,
    total_time INTEGER,
    PRIMARY KEY (id_mac)
);

-- Machine stats
CREATE TABLE IF NOT EXISTS MachineStat (
    id_mac INTEGER,
    piece_type INTEGER,
    piece_count INTEGER, 
    PRIMARY KEY (id_mac, piece_type),
    CHECK (piece_type>=1), CHECK (piece_type<=9),
    CHECK (id_mac>=0), CHECK (piece_type<=7),
    CONSTRAINT fk_mac FOREIGN KEY (id_mac) REFERENCES Machine(id_mac)
);