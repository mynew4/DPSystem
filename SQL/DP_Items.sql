CREATE TABLE `dp_items` (
	`item_id` INT(10) UNSIGNED NOT NULL COMMENT 'Item ID or Spell ID depending on type.',
	`item_type` TINYINT(3) UNSIGNED NOT NULL COMMENT '1 = Weapon, 2 = Armor, 3 = Misc; 4 = Spells.',
	`item_name` VARCHAR(255) NOT NULL COMMENT 'Item / Spell Name.',
	`quality` TINYINT(3) UNSIGNED NOT NULL COMMENT 'Unused, set to 0.',
	`dp_cost` INT(10) UNSIGNED NOT NULL COMMENT 'Cost to purchase in Donation Points.',
	PRIMARY KEY (`item_id`)
)
COMMENT='Fox - DPSystem'
COLLATE='utf8_general_ci'
ENGINE=MyISAM;

INSERT INTO dp_items
	(item_id, item_type, item_name, quality, dp_cost)
	VALUES (19019, 1, 'Thunderfury, Blessed Blade of the Windseeker', 0, 20);	
INSERT INTO dp_items
	(item_id, item_type, item_name, quality, dp_cost)
	VALUES (51227, 2, 'Sanctified Ymirjar Lords Helmet', 0, 30);
INSERT INTO dp_items
	(item_id, item_type, item_name, quality, dp_cost)
	VALUES (45801, 3, 'Reins of the Ironbound Proto-Drake', 0, 15);	
INSERT INTO dp_items
	(item_id, item_type, item_name, quality, dp_cost)
	VALUES (674, 4, 'Dual Wield', 0, 50);